#import "ParTok.h"
#import "Tok.h"


#import "DebugPrint.h"
#define PrintParTok(...) DebugPrint(__VA_ARGS__)


void ParTok::initToks(Par *par) {
    
    if (par->memoMe<Par::MemoNow) {
        par->memoMe=Par::MemoNow;
        
        int64_t hash = str2int(par->name.c_str());
        Tok::nameEnum[par->name] = hash;
        Tok::enumName[hash] = par->name;
        
        for (Par*par2 :par->pars) {
            initToks(par2);
        }
    }
}


void ParTok::parseBuf(const char *buf, bool trace, bool print) {
    
    Par::MemoNow++;
    Par::Trace=trace;
    initToks(root);
    ParDoc input((char*)buf);
    root->parse(tokens,input,0);
    if (print) {
        printToks();
    }
}

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
        
        PrintParTok("%2i,%2i:",row++,tok->level);
        
        for (int col = 0; col<tok->level; col++) {
            PrintParTok(" ");
        }
        
        TokType type = tok->tokType;
        const char* name = Tok::enumName[type].c_str();
        const char* value = tok->value->c_str();
        PrintParTok("%s : %s \n", name, value);
    }
}
