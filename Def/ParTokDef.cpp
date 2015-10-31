#import "ParTokDef.h"
#import "Tok.h"

void ParTokDef::initToks(Par*par) {

    int64_t num;

#define Par_(a,b)  num = str2int(#a); Tok::hashName[num] = #a; Tok::nameHash[#a] = num;
#include "Def.par"
#undef Par_
   
#define Par_(a,b) a = new Par(); a->setName(#a);
#include "Def.par" 
#undef Par_
    
#define Par_(a,b) { *a=*b; delete(b); }
#include "Def.par"
#undef Par_
    
    root = def;
}

