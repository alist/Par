typedef enum {
    
    kDef_undef ,
    // create enums from tr3.def.h
#define Tok_(a,b) kDef_##a,
#include "DefTokDef.h"
#undef Tok_
    kDef_MaxSize
}       
DefEnum;