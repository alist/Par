#import "DefParseDef.h"
#import "DebugPrint.h"
#define PrintDefParseDef(...) DebugPrint(__VA_ARGS__)

void DefParseDef::initParToks() {
    
#define Tok_(a,b) ParTok::enumName[kDef_##a] = #a; ParTok::nameEnum[#a] = kDef_##a;
#include "DefTokDef.h"
#undef Tok_
    
    if (tokens) {
        deleteParToks();
    }
    tokens = new ParToks();
    
    // a = new ParseLex(); a->setWho(#a);
#define Tok_(a,b) ParNew(a,b)
#include "DefTokDef.h"
#undef Tok_
    
    //  { *a=*b; delete(b); }
#define Tok_(a,b) ParBind(a,b)
#include "DefTokDef.h"
#undef Tok_
    
    ParDef::_tokens = tokens;
}

void DefParseDef::deleteParToks() {

    if (tokens) {
        
        for (ParTok *token : *tokens) {
            delete token;
        }
        tokens->clear();
        tokens = 0;
    }
}

void DefParseDef::printParToks() {
    
    int row=0;
    for (ParTok* tok : *tokens) {
        
        PrintDefParseDef("%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            PrintDefParseDef(" ");
        }
        
        TokType type = tok->tokType; ///???
        const char* name = ParTok::enumName[type].c_str();
        const char* value = tok->value->c_str();
        PrintDefParseDef("%s : %s \n", name, value);
    }
}

void DefParseDef::parseBuf(const char*buf, bool print) {
    
    initParToks();
    ParDoc input((char*)buf);
    par->parse(input,0); ///??? par->
    tokens->front()->tokType = 0;
    if (print) {
        printParToks();
    }
}