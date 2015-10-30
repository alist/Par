
#import "Tok.h"

#define PrintParTok(...) DebugPrint(__VA_ARGS__)

int Tok::TokIdMax = 0;
TokType Tok::TokTypeMax = 1000;
TokNameEnum Tok::nameEnum;    // translate token name string into enum type
TokEnumName Tok::enumName;    // translate token enum type into name string

Tok::Tok (string &name, const char*value_, int level_) {
    
    tokId = TokIdMax++;
    tokType = nameEnum[name];
    value = value_ ?  new string(value_) : new string();
    level = level_;
    
}
