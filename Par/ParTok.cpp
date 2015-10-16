#import "ParTok.h"
#import "Tok.h"
#import "Par.h"

#import "DebugPrint.h"
#define PrintDefParse(...) DebugPrint(__VA_ARGS__)

void ParTok::deleteToks() {

    if (tokens) {
        
        for (Tok *token : *tokens) {
            delete token;
        }
        tokens->clear();
        tokens = 0;
    }
}

void ParTok::printToks() {
    
    int row=0;
    for (Tok* tok : *tokens) {
        
        PrintDefParse("%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            PrintDefParse(" ");
        }
        
        TokType type = tok->tokType;
        const char* name = Tok::enumName[type].c_str();
        const char* value = tok->value->c_str();
        PrintDefParse("%s : %s \n", name, value);
    }
}
