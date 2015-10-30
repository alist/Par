#import "ParTok.h"
#import "ParMacro.h"

struct ParTokDef:ParTok {
    
    virtual void initToks(Par*);
   
#define Par_(a,b) ParDeclare(a,b) // Par *a;
#include "Def.par"
#undef Par_
    
};