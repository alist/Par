
#import "Tok.h"

#define PrintParTok(...) DebugPrint(__VA_ARGS__)

TokNameEnum Tok::nameHash;    // translate token name string into enum type
TokEnumName Tok::hashName;    // translate token enum type into name string

Tok::Tok (string &name, const char*charp, int level_) {
    
    setTokType(name);
    value = charp ?  new string(charp) : new string();
    level = level_;
}

Tok::Tok(string &name, const char*charp, int startIdx, int endIdx, int level_) {

    setTokType(name);
    value = new string(charp+startIdx,endIdx-startIdx+1);
    level = level_;
}

inline void Tok::setTokType(string&name) {
    
    tokType = nameHash[name];
    
    if (tokType==0) {
        int64_t hash = str2int(name.c_str());
        nameHash[name] = hash;
        hashName[hash] = name;
        tokType = hash;
    }
}