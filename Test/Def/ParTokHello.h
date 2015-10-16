#import "ParTok.h"
#import "Tok.h"
#import "Par.h"

/* enums are associated with lvalue of Tok_(a, ...) for example:
 * Tok_(par,Mny(One(lvalue),One(list)))
 * creates a kTok_par
 */
typedef enum {
    
    kHello_undef,
#define Tok_(a,b) kHello_##a,
#include "Hello.tok"
#undef Tok_
    kHello_MaxSize
} DefEnum_Hello;


struct ParTokHello: ParTok {
    
    void initToks();
    void parseBuf(const char*buf, bool print);

    // ParTokCal *a;
#define Tok_(a,b) ParDeclare(a,b)
#include "Hello.tok"
#undef Tok_
    
};
