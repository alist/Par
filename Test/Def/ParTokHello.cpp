#import "ParTokHello.h"

void ParTokHello::initToks() {
    
#define Tok_(a,b) ParEnum(a,b,Hello);
#include "Hello.tok"
#undef Tok_
    
    if (tokens) {
        deleteToks();
    }
    tokens = new Toks();
    
#define Tok_(a,b) ParNew(a,b)
#include "Hello.tok" // a = new ParseLex(); a->setWho(#a);
#undef Tok_
    
#define Tok_(a,b) ParBind(a,b)
#include "Hello.tok"  //  { *a=*b; delete(b); }
#undef Tok_
    
    Par::_tokens = tokens;
}

void ParTokHello::parseBuf(const char*buf, bool print) {
    
    initToks();
    ParDoc input((char*)buf);
    def->parse(input,0); // this is at the top of the Tok_(...) definitions in the *.tok file
    if (print) {
        printToks();
    }
}