#import "ParTokDef.h"
#import "Tok.h"

void ParTokDef::initToks() {

#define Tok_(a,b) ParEnum(a,b,Def)
#include "Def.tok"
#undef Tok_
    
    if (tokens) {
        deleteToks();
    }
    tokens = new Toks();
    
#define Tok_(a,b) ParNew(a,b)
#include "Def.tok" // a = new ParseLex(); a->setWho(#a);
#undef Tok_
    
#define Tok_(a,b) ParBind(a,b)
#include "Def.tok"  //  { *a=*b; delete(b); }
#undef Tok_
    
    Par::_tokens = tokens;
}


void ParTokDef::parseBuf(const char*buf, bool print) {
    
    initToks();
    ParDoc input((char*)buf);
    def->parse(input,0);
    if (print) {
        printToks();
    }
}