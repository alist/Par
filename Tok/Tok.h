/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import "TokTypes.h"

using namespace std;

struct Tok {
    
    // Tok(NameEnum|EnumName) is a consistent two-way hash shared by all grammars
    static TokNameEnum nameHash;    // translate token name string into enum type
    static TokEnumName hashName;    // translate token enum type into name string
    
    TokType tokType;                // of which tokType is assigned (there may be 0 or more in a doc)
    string *value;                  // some Toks have a value as a payload
    int level;                      // all Toks have position withing a parsing hierarchy
    
    Tok(){}
    Tok (string &name, const char*charp, int level_);
    Tok (string &name, const char*charp, int startIdx, int endIdx, int level_);
    void setTokType(string&name);
    
    static void printToks(Toks&toks, FILE *fp);
    
   ~Tok() {}
};
