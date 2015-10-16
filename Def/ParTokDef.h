#import "ParTok.h"
#import "Par.h"

typedef enum {
    kDef_undef,
#define Tok_(a,b) kDef_##a,
#include "Def.tok"
#undef Tok_
    kDef_MaxSize
} DefEnum_Def;


struct ParTokDef:ParTok {
    
    void initToks();
    void parseBuf(const char*buf, bool print);
   
    // Par *a;
#define Tok_(a,b) ParDeclare(a,b)
#include "Def.tok"
#undef Tok_
    
};