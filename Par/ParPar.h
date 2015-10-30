
#import "Tok.h"
#import "Par.h"
#import <string>
#import <unordered_map>

typedef unordered_map<std::string,Par*>NamePars;
struct ParTok;

struct ParPar {
    
    ParPar(){}
    void initWithBuf(const char*buf);
    void initWithFile(const char*filename);
    
    NamePars namePars;
    Pars parPars;
    ParTok *parTok;
    
    int List(Par*par,Toks*toks,int toki);
    void Lvalue(Toks*toks);
    void expandRvalue(Par*par);
    void promoteQuestion(Par *par);
    void fixupPar(Par*par);
    void fixup();
    void parseBuf(const char* buf);
    void parseFile(const char* filename);
};