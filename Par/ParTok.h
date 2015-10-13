
//#define PARSE_PAR

#import "ParNameEnum.h"

using namespace std;

struct ParTok {
    
    
    static ParNameEnum nameEnum;    // translate token name string into enum type
    static ParEnumName enumName;    // translate token enum type into name string
    static void initMapOnce();
    
    static int TokIdMax;            // while parsing a doc, each new token has a unique id
    int tokId;                      // that gets assigned to this token's tokId
    
    static TokType TokTypeMax;      // each lvalue of Tok_(a, (b c)) has a unique TokEnum
    TokType tokType;                // of which tokType is assigned (there may be 0 or more in a doc)
    string *value;                  // some ParToks have a value as a payload
    int level;                      // all ParToks have position withing a parsing hierarchy
    
    ParTok(){tokId = TokIdMax++;}
    ParTok (string &name, const char*value_, int level_);

   ~ParTok() {}
};
