#import "ParTok.h"
#import "ParDef.h"

/* enums are associated with lvalue of Tok_(a, ...) for example:
 * Tok_(par,Mny(One(lvalue),One(list)))
 * creates a kTok_par
 */
#import "DefEnumDef.h"

struct DefParseDef {
    
    ParToks *tokens;

    DefParseDef(){tokens=0;}
    ~DefParseDef(){deleteParToks();}
    
    void initParToks();
    void deleteParToks();
    void printParToks();
    
    void parseBuf(const char*buf, bool print);
   
    // ParDef *a;
#define Tok_(a,b) ParDeclare(a,b)
#include "DefTokDef.h"
#undef Tok_
    
};