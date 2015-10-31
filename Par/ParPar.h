
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
    ParList grammar;
    ParTok *parTok;
    
    void addPar(Toks*toks);
    int addList(Par*par,Toks*toks,int toki);
    void bindName(Par*par);
    void promoteOnlyChild(Par *par);
    void bindParTree(Par*par);
    void bindGrammar();
    void parseBuf(const char* buf);
    void parseFile(const char* filename);
};