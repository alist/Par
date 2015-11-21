
/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import "ParPar.h"
#import "ParFile.h"
#include <assert.h>
#import "Tok.h"

ParBoot ParPar::_ParBoot;

void ParPar::parFile2Grammar(const char*fname, FILE *fp) {
    
    Par::MemoNow++;
    
    const char *buf = ParFile::readInputFile(fname);
    if (buf) {
        parBuf2Grammar(buf,fp);
        delete (buf);
    }
    else {
        fprintf(stderr,"\n*** ParPar::initWithFile(%s) - file not found\n", fname);
        errors++;
    }
}

/* convert a char buffer in the format: "a (b c)\nb ('b')\nc ('c')"
 * into a token stream, which in turn gets parsed into a grammar
 */
void ParPar::parBuf2Grammar(const char*buf, FILE*fp) {
    
    if (buf) {
        
        grammar.clear();
        _bufGram2toks(buf,_ParBoot.grammar,fp);
        _toks2Grammar();
        _bindGrammar();
        _hashGrammar(grammar[0]);
        toks.clear();
    }
    else {
        fprintf(stderr,"\n*** ParPar::initWithBuf - null buffer\n");
        errors++;
    }
}

/* create a token stream by applying parGram to buf
 */
void ParPar::_bufGram2toks(const char *buf, ParList &gram, FILE *fp) {
    
    Par::MemoNow++;
    
    /* ParDoc shares buf with many instances of itself
     * to allow for backtracking while parsing a pattern tree */
    ParDoc doc((char*)buf);
    doc.eatWhitespace();
    doc.trimTrailspace();
    
    toks.clear();
    int level = 0;
    Par *before = 0;
    Par *root = gram.front();
    root->parse(toks,doc,level,before);
 }
/* parse  a (b c?)+
 * convert the token stream into a gramar
 * starting with an input file in the format of: a (b c
 * where a is the value of "par" and b c are the values of "list"
 */
void ParPar::_toks2Grammar() {
    
    Par *par = 0;
    
    for (int toki=0; toki < toks.size();) {
        
        Tok *tok = toks[toki];
        
        switch (tok->tokType) {
                
            case str2int("par"): {
                
                if (++toki < toks.size()) {
                    
                    tok = toks[toki];
                    string *name = tok->value;
                    par = new Par(name);
                    namePars[*name] = par;
                    grammar.push_back(par);
                }
                break;
            }
            case str2int("list"): {
                
                toki = _addList(par,toki);
                break;
            }
            default: {
                
                toki++;
                break;
            }
        }
    }
}
/* apply _BootGram grammar to new <file>.par
 */
int ParPar::_addList(Par*par,int toki) {
    
    Tok *tok = toks[toki];
    int level = tok->level;
    
    while (toki+1<toks.size() &&
           toks[toki+1]->level>level) {
        
        toki++;
        tok = toks[toki];
        
        switch (tok->tokType) {
                
            case str2int("par"): {
                
                return toki;
            }
                
            case str2int("waves"): {
                
                Par *pari = new Par(kMatchWave);
                par->parList.push_back(pari);
                toki = _addList(pari, toki);
                break;
            }
                
            case str2int("ands"): {
                
                Par *pari = new Par(kMatchAnd);
                par->parList.push_back(pari);
                toki = _addList(pari, toki);
                break;
            }
                
            case str2int("ors"): {
                
                Par *pari = new Par(kMatchOr);
                par->parList.push_back(pari);
                toki = _addList(pari, toki);
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
                toki = _addList(pari, toki);
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
                pari->maxRepeat = num;
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

/* Use by test suites to save a token stream to a file
 * and the subsequently compare the results.
 */
void ParPar::txtBuf2TokFile(const char *buf, const char *file) {
    
    // redirect trace stdout to file
    FILE *fp = freopen(file, "w", stdout);

    // parse the buffer
    txtBuf2Tokens(buf,fp);
    
    Tok::printToks(toks, fp);
    
    fflush(fp);
    fclose(fp);
}

/* Initialize name hash, which is used for "case str2int("name"): statements.
 * starting from root node (usually a "def(...)", parse the graph depth first
 * and add name and its hash to two unordered_maps.
 * Memoize to eliminate reprocessing cyclic loops
 */
void ParPar::_hashGrammar(Par *par) {
    
    // only pass through once - break cycles
    if (par->memoMe < Par::MemoNow) {
        par->memoMe = Par::MemoNow;
        
        /* each unique name will have only one hash
         * regardless of which grammar it is coming from
         * so this ordered_map can be shared globally
         */
        
        int64_t hash = str2int(par->name.c_str());
        
        if (Tok::hashName.find(hash)==Tok::hashName.end()) {
            
            Tok::nameHash[par->name] = hash;
            Tok::hashName[hash] = par->name;
        }
        else {
            // Failing this assert means a collision was detected
            // To fix, redesign str2int -- can't fix this during runtime
            assert(Tok::hashName[hash] == par->name);
        }
        for (Par*par2 :par->parList) {
            _hashGrammar(par2);
        }
    }
}

/* grammar is ready, convert text buffer into toks
 */
void ParPar::txtBuf2Tokens(const char *buf, FILE *fp) {
    
    // memoize the depth-first binding of symbol names
    Par::MemoNow++;

    if (grammar.size()>0) {
        
        _bufGram2toks(buf,grammar,fp);
    }
    else {
        fprintf(stderr,"\n*** ParPar::parseBuf has no tokens\n");
    }
}

/* grammar is ready, read a text file into buf and call txtBuf2Tokens
 */
void ParPar::txtFile2Tokens(const char *fname, FILE *fp, bool trace) {
    
    Par::MemoNow++;
    Par::Trace = trace;
    
    char *buf = ParFile::readInputFile(fname);
    if (buf) {
        txtBuf2Tokens(buf,fp);
        free(buf);
    }
    else {
        fprintf(stderr,"\n*** ParPar::parseFile(%s) - file not found\n", fname);
        errors++;
    }
    Par::Trace = false;
}


/* find named par and hoist its parList to be my parList
 * but keep my repetition spec
 */
inline void ParPar::_bindName(Par*par) {
    
    string &name = par->name;
    Par *pari = namePars[name];
    
    // b in (a ~b c)
    if (!pari && name[0]=='~') {
    
        pari = namePars[name.substr(1)];
    }
    
    if (pari) { // is this not a par?
        
        if (par->matching == kMatchAhead) {
            
            Par *par2 = new Par(&name);
            _bindName(par2);
            par->parList.push_back(par2);
        }
        else if (pari!=par) {
            
            par->parList  = pari->parList;     // replace with children with lvalue's children
            par->match    = pari->match;
            par->matching = pari->matching;
        }
    }
    else if (name!="?" && par->matching!=kMatchAhead) {
        errors++;
        fprintf(stderr,"\n*** ParPar::_bindName: '%s' not found\n",name.c_str());
     }
}

/* Some Pars have a single child after ? ,
 * so promote the child to take the Par's place
 */
void ParPar::_promoteOnlyChild(Par *par) {
    
    if (par->parList.size()==1) {
        
        Par *par2 = par->parList.back();
        
        if (par2->name=="?") {
            
            /* the default Par is a single instance (min|max)Count==1 
             * so overide with the child's repetition
             */
            if (par->minRepeat==1 && par->maxRepeat==1) {
                par->minRepeat = par2->minRepeat;
                par->maxRepeat = par2->maxRepeat;
            }
            par->matching = par2->matching;
            par->match    = par2->match;
            par->parList  = par2->parList;
        }
    }
}
/* lookup each name in a par->parList 
 * to bind to the actual par of that name (lvalue)
 */
void ParPar::_bindGrammarTree(Par*par) {
    
    if (par->memoMe < Par::MemoNow) {
        par->memoMe = Par::MemoNow;
        
        _promoteOnlyChild(par);
        _bindName(par);
        
        if (errors>0) {
            fprintf(stderr, "\n*** Exiting Par due to %i error%s \n\n",errors,errors>1 ? "s." : ".");
            exit(-1);
        }

        if (par->parList.size()>0) {
            
            for (Par*par2 : par->parList) {
                
                _bindGrammarTree(par2);
            }
        }
    }
}

void ParPar::_bindGrammar() {
    
    for (int i=0; i<grammar.size(); i++) {
        
        Par*par = grammar[i];
        _promoteOnlyChild(par);
    }
    Par::MemoNow++;
    for (Par*par : grammar) {
        
        _bindGrammarTree(par);
    }
}
