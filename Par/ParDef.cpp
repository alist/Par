#import "ParDef.h"
#import "ParTokDef.h"

#import "DebugPrint.h"
#define PrintParDef(...) DebugPrint(__VA_ARGS__)

void ParDef::initWithFile(const char*filename) {
    
    Par::MemoNow++;
    
    const char *buf = readFile(filename);
    if (buf) {
        initWithBuf(buf);
        delete (buf);
    }
    else {
        DebugPrint("*** ParDef::initWithFile(%s) - file not found\n", filename);
    }
}
void ParDef::initWithBuf(const char*buf) {

    if (buf) {
        
        parTok = 0;
        ParTokDef *parseDef = new ParTokDef();
        Toks *toks = parseDef->buf2tok(buf,/*trace*/true,/*print*/true);
        addPar(toks);
        bindGrammar();
    }
    else {
        DebugPrint("*** ParDef::initWithBuf - null buffer\n");
    }
}

Toks *ParDef::parseFile(const char *filename) {

    Par::MemoNow++;

    char *buf = readFile(filename);
    if (buf) {
        Toks *toks = parseBuf(buf);
        free(buf);
        return toks;
    }
    else {
        DebugPrint("*** ParDef::parseFile(%s) - file not found\n", filename);
        return 0;
    }
}
/*
 */
Toks *ParDef::parseBuf(const char *buf) {
    
    if (grammar.size()>0) {
        
        parTok = new ParTok(grammar[0]);
        Toks *toks = parTok->buf2tok(buf,/*trace*/true,/*print*/true);
        return toks;
    }
    else {
        DebugPrint("*** ParDef::parseBuf has no tokens\n");
        return 0;
    }
}

void ParDef::addPar(Toks*toks) {
    
    Par *par = 0;
    
    for (int toki=0; toki<toks->size();) {
        
        Tok *tok = (*toks)[toki];
        
        switch (tok->tokType) {
                
            case str2int("par"): {
                
                if (++toki < toks->size()) {
                    
                    tok = (*toks)[toki];
                    string *name = tok->value;
                    par = new Par(name);
                    namePars[*name] = par;
                    grammar.push_back(par);
                }
                break;
            }
            case str2int("list"): {
                
                toki = addList(par,toks,toki);
                break;
            }
            default: {
                
                toki++;
                break;
            }
        }
    }
}

int ParDef::addList(Par*par,Toks*toks,int toki) {
    
    Tok *tok = (*toks)[toki];
    int level = tok->level;
    
    while (toki+1<toks->size() &&
           (*toks)[toki+1]->level>level) {
        
        toki++;
        tok = (*toks)[toki];
        
        switch (tok->tokType) {
                
            case str2int("par"): {
                
                return toki;
            }
                
            case str2int("waves"): {
                
                Par *pari = new Par(kMatchWave);
                par->parList.push_back(pari);
                toki = addList(pari, toks, toki);
                break;
            }
            case str2int("ands"): {
                
                Par *pari = new Par(kMatchAnd);
                par->parList.push_back(pari);
                toki = addList(pari, toks, toki);
                break;
            }

            case str2int("ors"): {
                
                Par *pari = new Par(kMatchOr);
                par->parList.push_back(pari);
                toki = addList(pari, toks, toki);
                break;
            } 
                
            case str2int("name"): {
                
                string *name = tok->value;
                Par *pari = new Par(name);
                par->parList.push_back(pari);
                break;
            }
                
            case str2int("quote"): {
                
                string *value = tok->value;
                ParQuo *quo = new ParQuo(value->c_str());
                Par*pari = new Par(quo);
                par->parList.push_back(pari);
                break;
            }
                
            case str2int("regx"): {
                
                string *value = tok->value;
                ParRegx *rx = new ParRegx(value->c_str());
                Par*pari = new Par(rx);
                par->parList.push_back(pari);
                break;
            }
            case str2int("pred"): {
                
                Par*pari = par->parList.back();
                if (pari->matching==kMatchRegx) {
                    ParRegx *rx = pari->match.regx;
                    rx->result2 = *tok->value;
                }
                break;
            }
            case str2int("meta"): {
                
                Par *pari = new Par(kMatchMeta);
                pari->name = *tok->value;
                par->parList.push_back(pari);
                toki = addList(pari, toks, toki);

//                ParRegx *rx = new ParRegx("'^\\w+");
//                Par*pari = new Par(rx);
//                pari->name = *tok->value;
//                pari->matching = kMatchMeta;
//                par->parList.push_back(pari);
//                
                break;
            }
                
            case str2int("opt"): {
                
                Par *pari = par->parList.back();
                pari->repeat= kRepOpt;
                break;
            }
                
            case str2int("any"): {
                
                Par *pari = par->parList.back();
                pari->repeat = kRepAny;
                break;
            }
            case str2int("mny"): {
                
                Par *pari = par->parList.back();
                pari->repeat = kRepMny;
                break;
            }
        }
    }
    return toki;
}

/* find named par and get its parList
 */
inline void ParDef::bindName(Par*par) {
    
    string &name = par->name;
    Par *pari = namePars[name];
    
    if (pari) {        // is this not a par?
        
        if (par->matching == kMatchMeta) {
            
            Par *par2 = new Par(&name);
            bindName(par2);
            par->parList.push_back(par2);
        }
        else if (pari!=par) {
            
            par->parList  = pari->parList;     // replace with children with lvalue's children
            par->match    = pari->match;
            par->matching = pari->matching;
            
            if (par->repeat==kRepOne) { ///???
                par->repeat = pari->repeat;
            }
        }
    }
    else if (name!="?" &&
             par->matching!=kMatchMeta){
        DebugPrint("*** ParDef::bindName: '%s' not found\n",name.c_str());
     }
}

/* promote only child
 */
void ParDef::promoteOnlyChild(Par *par) {
    
    if (par->parList.size()==1) {
        
        Par *par2 = par->parList.back();
        
        if (par2->name=="?") {
            if (par->repeat==kRepOne) {
                par->repeat = par2->repeat;
            }
            par->matching = par2->matching;
            par->match    = par2->match;
            par->parList  = par2->parList;
        }
    }
}
void ParDef::bindParTree(Par*par) {
    
    if (par->memoMe < Par::MemoNow) {
        par->memoMe = Par::MemoNow;
        
        promoteOnlyChild(par);
        bindName(par);
        
        if (par->parList.size()>0) {
            
            for (Par*par2 : par->parList) {
                
                bindParTree(par2);
            }
        }
    }
}
void ParDef::bindGrammar() {
    
    for (int i=0; i<grammar.size(); i++) {
        
        Par*par = grammar[i];
        promoteOnlyChild(par);
    }
    Par::MemoNow++;
    for (Par*par : grammar) {
        
        bindParTree(par);
    }
}
#pragma mark - file

char * ParDef::readFile(const char*inputFile) {
    
    FILE *file = freopen(inputFile, "r", stderr);
    
    if (file) {
        
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buf = (char*)malloc(fileSize+1);
        fread((void*)buf, fileSize, 1, file);
        buf[fileSize]='\0';
        return buf;
    }
    else {
        PrintParDef("\n *** file:%s not found",inputFile);
        return 0;
    }
}




