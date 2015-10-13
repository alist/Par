
#import "ParTok.h"

#define PrintParTok(...) DebugPrint(__VA_ARGS__)

int ParTok::TokIdMax = 0;
TokType ParTok::TokTypeMax = 1000;
ParNameEnum ParTok::nameEnum;    // translate token name string into enum type
ParEnumName ParTok::enumName;    // translate token enum type into name string

void ParTok::initMapOnce() {
    
}

ParTok::ParTok (string &name, const char*value_, int level_) {
    
    tokId = TokIdMax++;
    tokType = nameEnum[name];
    value = value_ ?  new string(value_) : new string();
    level = level_;
    
}
