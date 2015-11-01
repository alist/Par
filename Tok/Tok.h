
#import "TokTypes.h"

using namespace std;

struct Tok {
    
    static TokNameEnum nameHash;    // translate token name string into enum type
    static TokEnumName hashName;    // translate token enum type into name string
    
    TokType tokType;                // of which tokType is assigned (there may be 0 or more in a doc)
    string *value;                  // some Toks have a value as a payload
    int level;                      // all Toks have position withing a parsing hierarchy
    
    Tok(){}
    Tok (string &name, const char*value_, int level_);

   ~Tok() {}
};
