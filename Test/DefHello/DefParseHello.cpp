#import "DefParseHello.h"
#import "DebugPrint.h"
#define PrintDefCalParse(...) DebugPrint(__VA_ARGS__)

void DefParseHello::initParToks() {
    
#define Tok_(a,b) ParTok::enumName[kCal_##a] = #a; ParTok::nameEnum[#a] = kCal_##a;
#include DefHello.h"
#undef Tok_
    
    if (tokens) {
        deleteParToks();
    }
    tokens = new ParToks();
    
    // a = new ParseLex(); a->setWho(#a);
#define Tok_(a,b) ParNew(a,b)
#include "DefHello.h"
#undef Tok_
    
    //  { *a=*b; delete(b); }
#define Tok_(a,b) ParBind(a,b)
#include DefHello.h"
#undef Tok_
    
    ParDef::_tokens = tokens;
}

void DefParseHello::deleteParToks() {

    if (tokens) {
        
        for (ParTok *token : *tokens) {
            delete token;
        }
        tokens->clear();
        tokens = 0;
    }
}

void DefParseHello::printParToks() {
    
    int row=0;
    for (ParTok* tok : *tokens) {
        
        PrintDefCalParse("%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            PrintDefCalParse(" ");
        }
        
        TokType type = tok->tokType; ///???
        const char* name = ParTok::enumName[type].c_str();
        const char* value = tok->value->c_str();
        PrintDefCalParse("%s : %s \n", name, value);
    }
}

void DefParseHello::parseBuf(const char*buf, bool print) {
    
    initParToks();
    ParDoc input((char*)buf);
    par->parse(input,0); ///??? par->
    tokens->front()->tokType = 0;
    if (print) {
        printParToks();
    }
}