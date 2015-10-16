#import "ParTok.h"
#import "Tok.h"
#import "Par.h"

typedef enum {
    kDef_undef,
#define Tok_(a,b) kCal_##a,
#include "Cal.tok"
#undef Tok_
    kCal_MaxSize
} DefEnum_Cal;

struct ParTokCal:ParTok {
    
    void initToks();
    void parseBuf(const char*buf, bool print);
    
    // ParTokCal *a;
#define Tok_(a,b) ParDeclare(a,b)
#include "Cal.tok"
#undef Tok_
    
};