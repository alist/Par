/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "ParParToks.h"
#import "ParBoot.h"
#import "Tok.h"

/* create the ParPar grammar to parse grammars via Par.boot
 */
void ParParToks::initNameHash(Par*ignore) {

    int64_t hash;
    
#define Par_(a,b) \
hash = str2int(#a); Tok::hashName[hash] = #a; Tok::nameHash[#a] = hash; \
Par *a = new Par(); a->setName(#a); parParList.push_back(a);
#include "Par.boot"
#undef Par_
    
#define Par_(a,b) { *a=*b; delete(b); }
#include "Par.boot"
#undef Par_
    
    root = parParList[0];
}

