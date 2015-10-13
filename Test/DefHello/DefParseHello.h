#import "ParTok.h"
#import "ParDef.h"

/* enums are associated with lvalue of Tok_(a, ...) for example:
 * Tok_(par,Mny(One(lvalue),One(list)))
 * creates a kTok_par
 */
#import "DefEnumHello.h"

struct DefParseHello {
    
    ParToks *tokens;

    DefParseHello(){tokens=0;}
    ~DefParseHello(){deleteParToks();}
    
    void initParToks();
    void deleteParToks();
    void printParToks();
    
    void parseBuf(const char*buf, bool print);
   
    // ParCal *a;
#define Tok_(a,b) ParDeclare(a,b)
#include "DefHello.h"
#undef Tok_
    
};