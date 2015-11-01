#import "ParTok.h"
#import "ParMacro.h"

struct ParTokDef:ParTok {
    
    virtual void initNameHash(Par*);
   
#define Def_(a,b) Par *a;
#include "Def.par"
#undef Def_
    
};