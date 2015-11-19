/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "ParToks.h"
#import "Tok.h"
#include <assert.h>

#import "DebugPrint.h"
#define PrintParToks(...) DebugPrint(__VA_ARGS__)

/* Initialize name hash, which is used for "case str2int("name"): statements.
 * starting from root node (usually a "def(...)", parse the graph depth first
 * and add name and its hash to two unordered_maps.
 * Memoize to eliminate reprocessing cyclic loops
 */
void ParToks::initNameHash(Par *par) {
    
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
            initNameHash(par2);
        }
    }
}

/* buf2tok is invoked twice:
 * 1) ParParToks creates the grammar to parse grammars (via buf2Grammar)
 * 2) ParToks uses the grammar to parse texts (via parseBuf)
 */
Toks *ParToks::buf2tok(const char *buf, bool trace, bool print, FILE *fp) {
    
    Par::MemoNow++;
    Par::Trace=trace;
    
    ///??? initNameHash(root);
    
    ParDoc doc((char*)buf);
    root->parseStart(toks, doc);
    if (print) {
        printToks(fp);
    }
    return toks;
}

ParToks::~ParToks() {

    if (toks) {
        
        for (Tok *token : *toks) {
            delete token;
        }
        toks->clear();
        toks = 0;
    }
}

void ParToks::printToks(FILE *fp) {
    
    fprintf(fp,"\n");
    
    int row=0;
    for (Tok* tok : *toks) {
        
        fprintf(fp,"%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            fprintf(fp," ");
        }
        
        TokType type = tok->tokType;
        const char* name = Tok::hashName[type].c_str();
        const char* value = tok->value->c_str();
        fprintf(fp,"%s : %s \n", name, value);
    }
    fprintf(fp,"\n");

}
