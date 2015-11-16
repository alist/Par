#import "ParDef.h"
#import "ParTokDef.h"
#import "ParFile.h"

#import "DebugPrint.h"
#define PrintParDef(...) DebugPrint(__VA_ARGS__)

void ParDef::initWithFile(const char*filename) {
    
    Par::MemoNow++;
    
    const char *buf = ParFile::readInputFile(filename);
    if (buf) {
        initWithBuf(buf);
        delete (buf);
    }
    else {
        fprintf(stderr,"\n*** ParDef::initWithFile(%s) - file not found\n", filename);
        errors++;
    }
}

void ParDef::initWithBuf(const char*buf) {

    if (buf) {
        
        parTok = 0;
        ParTokDef *parTok = new ParTokDef();
        Toks *toks = parTok->buf2tok(buf,traceBuf,printToks);
        addPar(toks);
        bindGrammar();
    }
    else {
         fprintf(stderr,"\n*** ParDef::initWithBuf - null buffer\n");
        errors++;
    }
}
Toks *ParDef::parseFile(const char *filename) {

    Par::MemoNow++;

    char *buf = ParFile::readInputFile(filename);
    if (buf) {
        Toks *toks = parseBuf(buf);
        free(buf);
        return toks;
    }
    else {
        fprintf(stderr,"\n*** ParDef::parseFile(%s) - file not found\n", filename);
        errors++;
        return 0;
    }
}

/*
 */
Toks *ParDef::parseBufTrace(const char *buf, const char *file) {
    
    // redirect trace stdout to file
    ParFile::redirectStdout(file);

    // push trace/print state
    bool pushTraceBuf = traceBuf;
    bool pushPrintToks = printToks;
    traceBuf = false;
    printToks = true;

    // parse the buffer
    Toks * toks = parseBuf(buf);
    
    // pop trace/print state
    ParFile::revertStdout();
    traceBuf = pushTraceBuf;
    printToks = pushPrintToks;
    
    return toks;
}

Toks *ParDef::parseBuf(const char *buf) {
        
    Par::MemoNow++;

    if (grammar.size()>0) {
        
        parTok = new ParTok(grammar[0]);
        parTok->buf2tok(buf,traceBuf,printToks);
    }
    else {
        DebugPrint("\n*** ParDef::parseBuf has no tokens\n");
        return 0;
    }
    return parTok->tokens;
}

/* Read the grammar, line by line, in the format of: a (b c)
 * where a is the value of "par" and b c are the values of "list"
 */
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
            case str2int("ahead"): {
                
                Par *pari = new Par(kMatchAhead);
                pari->name = *tok->value;
                par->parList.push_back(pari);
                toki = addList(pari, toks, toki);
                break;
            }
            case str2int("exact"): {
                
                int num = atoi(tok->value->c_str());
                Par *pari = par->parList.back();
                pari->setMinMax(num,num);
                break;
            }
            case str2int("min"): {
                
                int num = atoi(tok->value->c_str());
                Par *pari = par->parList.back();
                pari->setMinMax(num,-1); // default to infinit
                break;
            }
            case str2int("max"): {
                
                int num = atoi(tok->value->c_str());
                Par *pari = par->parList.back();
                pari->maxCount = num;
                break;
            }
            case str2int("opt"): {
                
                Par *pari = par->parList.back();
                pari->setMinMax(0,1);
                break;
            }
            case str2int("any"): {
                
                Par *pari = par->parList.back();
                pari->setMinMax(0,-1);
                break;
            }
            case str2int("mny"): {
                
                Par *pari = par->parList.back();
                pari->setMinMax(1,-1);
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
    if (!pari && name[0]=='~') {
    
        pari = namePars[name.substr(1)];
    }
    
    if (pari) { // is this not a par?
        
        if (par->matching == kMatchAhead) {
            
            Par *par2 = new Par(&name);
            bindName(par2);
            par->parList.push_back(par2);
        }
        else if (pari!=par) {
            
            par->parList  = pari->parList;     // replace with children with lvalue's children
            par->match    = pari->match;
            par->matching = pari->matching;
            
            //TODO: what is this???
            if (par->minCount==1 && par->maxCount==1) { ///???
                par->minCount = pari->minCount;
                par->maxCount = pari->maxCount;
            }
        }
    }
    else if (name!="?" && par->matching!=kMatchAhead) {
        errors++;
        fprintf(stderr,"\n*** ParDef::bindName: '%s' not found\n",name.c_str());
     }
}

/* promote only child
 */
void ParDef::promoteOnlyChild(Par *par) {
    
    if (par->parList.size()==1) {
        
        Par *par2 = par->parList.back();
        
        if (par2->name=="?") {
            
            //TODO: what is this???
            if (par->minCount==1 && par->maxCount==1) { ///???
                par->minCount = par2->minCount;
                par->maxCount = par2->maxCount;
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
        
        if (errors>0) {
            fprintf(stderr, "\n*** Exiting Par due to %i error%s \n\n",errors,errors>1 ? "s." : ".");
            exit(-1);
        }

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
