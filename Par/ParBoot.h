/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import "ParBoot.macros"
#import "Tok.h"

struct ParBoot {
  
    ParList grammar;
    
    ParBoot() {
        
        int64_t hash;
        
        //1st pass: setup lvalues of boot grammar
        
#define Par_(a,b) \
hash = str2int(#a); Tok::hashName[hash] = #a; Tok::nameHash[#a] = hash; \
Par *a = new Par(); a->setName(#a); grammar.push_back(a);
#include "Par.boot"
#undef Par_
        
        //2nd pass: bind rvalue of boot grammar
        
#define Par_(a,b) { *a=*b; delete(b); }
#include "Par.boot"
#undef Par_
    }
    
    ~ParBoot() {
        
        /* ParBoot owns its grammar Pars */
        
        for (Par *par : grammar) {
            delete par;
        }
    }
};

