/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

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


/* print the token stream that resulted from
 * applying a grammar to an input file
 */
void Tok::printToks(Toks &toks, FILE *fp) {
    
    fprintf(fp,"\n");
    
    int row=0;
    for (Tok* tok : toks) {
        
        fprintf(fp,"%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            fprintf(fp," ");
        }
        
        TokType type = tok->tokType;
        const char* name = Tok::hashName[type].c_str();
        const char* value = tok->value->c_str();
        fprintf(fp,"%s : %s \n", name, value);
    }
    fprintf(fp,"\n");
    
}
