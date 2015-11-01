
#import "Tok.h"

#define PrintParTok(...) DebugPrint(__VA_ARGS__)

TokNameEnum Tok::nameHash;    // translate token name string into enum type
TokEnumName Tok::hashName;    // translate token enum type into name string

Tok::Tok (string &name, const char*value_, int level_) {
    
    tokType = nameHash[name];
    value = value_ ?  new string(value_) : new string();
    level = level_;
}
