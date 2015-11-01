#import "ParTok.h"
#import "Tok.h"

#import "DebugPrint.h"
#define PrintParTok(...) DebugPrint(__VA_ARGS__)


/* Initialize name hash, which is used for "case str2int("name"): statements.
 * starting from root node (usually a "def(...)", parse the graph depth first
 * and add name and its hash to two unordered_maps.
 * Memoize to eliminate reprocessing cyclic loops
 */
void ParTok::initNameHash(Par *par) {
    
    // only pass through once - break cycles
    if (par->memoMe < Par::MemoNow) {
        par->memoMe = Par::MemoNow;
        
        /* each unique name will have only one hash
         * regardless of which grammar it is coming from
         * so this ordered_map can be shared globally
         */
        int64_t hash = str2int(par->name.c_str());
        Tok::nameHash[par->name] = hash;
        Tok::hashName[hash] = par->name;
        
        for (Par*par2 :par->parList) {
            initNameHash(par2);
        }
    }
}


Toks *ParTok::buf2tok(const char *buf, bool trace, bool print) {
    
    Par::MemoNow++;
    Par::Trace=trace;
    
    initNameHash(root);
    
    ParDoc input((char*)buf);
    root->parse(tokens,input,0);
    if (print) {
        printToks();
    }
    return tokens;
}

void ParTok::deleteToks() {

    if (tokens) {
        
        for (Tok *token : *tokens) {
            delete token;
        }
        tokens->clear();
        tokens = 0;
    }
}

void ParTok::printToks() {
    
    int row=0;
    for (Tok* tok : *tokens) {
        
        PrintParTok("%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            PrintParTok(" ");
        }
        
        TokType type = tok->tokType;
        const char* name = Tok::hashName[type].c_str();
        const char* value = tok->value->c_str();
        PrintParTok("%s : %s \n", name, value);
    }
}
