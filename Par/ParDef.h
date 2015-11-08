
#import "Tok.h"
#import "Par.h"
#import <string>
#import <unordered_map>

typedef unordered_map<std::string,Par*>NamePars;
struct ParTok;

struct ParDef {
    
    NamePars namePars;
    ParList grammar;
    ParTok *parTok;
    int errors;
    
    ParDef(){errors=0;}
    void initWithBuf(const char*buf);
    void initWithFile(const char*filename);
    
    Toks *parseBuf(const char* buf);
    Toks *parseFile(const char* filename);

    void addPar(Toks*toks);
    int addList(Par*par,Toks*toks,int toki);
 
    void bindName(Par*par);
    void promoteOnlyChild(Par *par);
    void bindParTree(Par*par);
    void bindGrammar();
    
    static char *readFile (const char*inputFile);

};