
#import "TokNameEnum.h"

using namespace std;

struct Tok {
    
    static TokNameEnum nameEnum;    // translate token name string into enum type
    static TokEnumName enumName;    // translate token enum type into name string
    
    static int TokIdMax;            // while parsing a doc, each new token has a unique id
    int tokId;                      // that gets assigned to this token's tokId
    
    static TokType TokTypeMax;      // each lvalue of Par_(a, (b c)) has a unique TokEnum
    TokType tokType;                // of which tokType is assigned (there may be 0 or more in a doc)
    string *value;                  // some Toks have a value as a payload
    int level;                      // all Toks have position withing a parsing hierarchy
    
    Tok(){tokId = TokIdMax++;}
    Tok (string &name, const char*value_, int level_);

   ~Tok() {}
};
