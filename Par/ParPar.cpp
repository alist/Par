#import "ParPar.h"
#import "ParTokDef.h"

#import "DebugPrint.h"
#define PrintParPar(...) DebugPrint(__VA_ARGS__)

void ParPar::initWithBuf(const char*buf) {

    if (buf) {
        
        parTok = 0;
        ParTokDef *parseDef = new ParTokDef();
        parseDef->parseBuf(buf,/*trace*/true,/*print*/true);
        
        Lvalue(parseDef->tokens);
        fixup();
    }
    else {
        DebugPrint("*** ParPar::initWithBuf - null buffer\n");
    }
}

void ParPar::initWithFile(const char*filename) {
    
    const char *buf = Par::readFile(filename);
    if (buf) {
        initWithBuf(buf);
        delete (buf);
    }
    else {
        DebugPrint("*** ParPar::initWithFile(%s) - file not found\n", filename);
    }
}

int ParPar::List(Par*par,Toks*toks,int toki) {
    
    Tok *tok = (*toks)[toki];
    int level = tok->level;
    
    while (toki+1<toks->size() &&
           (*toks)[toki+1]->level>level) {
        
        toki++;
        tok = (*toks)[toki];
        
        switch (tok->tokType) {
                
            case str2int("lvalue"): {
                
                return toki;
            }
                
            case str2int("ands"): {
                
                Par *pari = new Par(kMatchAnd);
                par->pars.push_back(pari);
                toki = List(pari, toks, toki);
                break;
            }
                
            case str2int("ors"): {
                
                Par *pari = new Par(kMatchOr);
                par->pars.push_back(pari);
                toki = List(pari, toks, toki);
                break;
            }
                
            case str2int("name"): {
                
                string *name = tok->value;
                Par *pari = new Par(name);
                par->pars.push_back(pari);
                break;
            }
                
            case str2int("quote"): {
                
                string *value = tok->value;
                ParQuo *quo = new ParQuo(value->c_str());
                Par*pari = new Par(quo);
                par->pars.push_back(pari);
                break;
            }
                
            case str2int("regx"): {
                
                string *value = tok->value;
                ParRegx *rx = new ParRegx(value->c_str());
                Par*pari = new Par(rx);
                par->pars.push_back(pari);
                break;
            }
                
            case str2int("opt"): {
                
                Par *pari = par->pars.back();
                pari->repeat= kRepOpt;
                break;
            }
                
            case str2int("any"): {
                
                Par *pari = par->pars.back();
                pari->repeat = kRepAny;
                break;
            }
            case str2int("mny"): {
                
                Par *pari = par->pars.back();
                pari->repeat = kRepMny;
                break;
            }
        }
    }
    return toki;
}

void ParPar::Lvalue(Toks*toks) {
    
    Par *par = 0;
    
    for (int toki=0; toki<toks->size();) {
        
        Tok *tok = (*toks)[toki];
        
        switch (tok->tokType) {
                
            case str2int("lvalue"): {
                
                if (++toki < toks->size()) {
                    
                    tok = (*toks)[toki];
                    string *name = tok->value;
                    par = new Par(name);
                    namePars[*name] = par;
                    parPars.push_back(par);
                }
                break;
            }
            case str2int("list"): {
                
                toki = List(par,toks,toki);
                break;
            }
            default: {
                
                toki++;
                break;
            }
        }
    }
}

// replace rvalue with lvalue's edges (pars)
inline void ParPar::expandRvalue(Par*par) {
    
    string &name = par->name;
    Par *pari = namePars[name];
    
    if (pari) {        // is this not a lvalue?
        
        if (pari!=par) {
            
            par->pars = pari->pars;     // replace with children with lvalue's children
            par->match = pari->match;
            par->matching = pari->matching;
            
            if (par->repeat==kRepOne) { ///???
                par->repeat = pari->repeat;
            }
        }
    }
    else if (name!="?"){
        DebugPrint("*** ParPar::expandRvalue: '%s' not found\n",name.c_str());
     }
}

/* promote single child of leaf node, example:
 * 'hello'->pars[0] == {'?', match.regx('hello');}
 */
void ParPar::promoteQuestion(Par *par) {
    
    if (par->pars.size()==1) {
        
        Par *par2 = par->pars.back();
        
        if (par2->name=="?") {
            if (par->repeat==kRepOne) { ///???
                par->repeat = par2->repeat;
            }
            par->matching = par2->matching;
            par->match = par2->match;
            par->pars = par2->pars;
        }
    }
}

void ParPar::fixupPar(Par*par) {
    
    if (par->memoMe < Par::MemoNow) {
        par->memoMe = Par::MemoNow;
        
        promoteQuestion(par);
        expandRvalue(par);
        
        if (par->pars.size()>0) {
            
            for (Par*par2 : par->pars) {
                
                fixupPar(par2);
            }
        }
    }
}

void ParPar::fixup() {
    
    for (int i=0; i<parPars.size(); i++) {
        
        Par*par = parPars[i];
        promoteQuestion(par);
    }
    Par::MemoNow++;
    for (Par*par : parPars) {
        
        fixupPar(par);
    }
}

void ParPar::parseBuf(const char *buf) {
    
    if (parPars.size()>0) {
        
        parTok = new ParTok(parPars[0]);
        parTok->parseBuf(buf,/*trace*/true,/*print*/true);
    }
    else {
        DebugPrint("*** ParPar::parseBuf has no tokens\n");
    }
}

void ParPar::parseFile(const char *filename) {
    
    char *buf = Par::readFile(filename);
    if (buf) {
        parseBuf(buf);
    }
    else {
        DebugPrint("*** ParPar::parseFile(%s) - file not found\n", filename);
    }
}




