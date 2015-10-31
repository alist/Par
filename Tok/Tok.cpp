
#import "Tok.h"

#define PrintParTok(...) DebugPrint(__VA_ARGS__)

int Tok::TokIdMax = 0;

TokNameEnum Tok::nameHash;    // translate token name string into enum type
TokEnumName Tok::hashName;    // translate token enum type into name string

Tok::Tok (string &name, const char*value_, int level_) {
    
    tokId = TokIdMax++;
    tokType = nameHash[name];
    value = value_ ?  new string(value_) : new string();
    level = level_;
    
}
