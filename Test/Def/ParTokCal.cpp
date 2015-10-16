#import "ParTokCal.h"
#import "Tok.h"

void ParTokCal::initToks() {
    
#define Tok_(a,b) ParEnum(a,b,Cal);
#include "Cal.tok"
#undef Tok_
    
    if (tokens) {
        deleteToks();
    }
    tokens = new Toks();
    
#define Tok_(a,b) ParNew(a,b)
#include "Cal.tok"  // a = new ParseLex(); a->setWho(#a);
#undef Tok_
    
#define Tok_(a,b) ParBind(a,b)
#include "Cal.tok"  //  { *a=*b; delete(b); }
#undef Tok_
    
    Par::_tokens = tokens;
}

void ParTokCal::parseBuf(const char*buf, bool print) {
    
    initToks();
    ParDoc input((char*)buf);
    def->parse(input,0);
    if (print) {
        printToks();
    }
}