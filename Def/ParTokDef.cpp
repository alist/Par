#import "ParTokDef.h"
#import "Tok.h"

void ParTokDef::initNameHash(Par*par) {

    int64_t hash;
#define Def_(a,b)  hash = str2int(#a); Tok::hashName[hash] = #a; Tok::nameHash[#a] = hash;
#include "Def.par"
#undef Def_
   
#define Def_(a,b) a = new Par(); a->setName(#a);
#include "Def.par" 
#undef Def_
    
#define Def_(a,b) { *a=*b; delete(b); }
#include "Def.par"
#undef Def_
    
    root = def;
}

