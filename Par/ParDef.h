#import "Tok.h"
#import "Par.h"
#import <string>
#import <unordered_map>

typedef unordered_map<std::string,Par*>NamePars;

struct ParTok;

struct ParDef {
    
    NamePars namePars;  // namePars[*name] = par
    ParList grammar;    // list of Pars that refer to each other
    ParTok *parTok;     // invoke grammar on input buffer to produce steam of tokens
    bool traceBuf;      // option to show sliding window of input buffer while tokenizing
    bool printToks;     // option to print resulting tokenstream
    int errors;         // catchall error
    
    ParDef() {
        
        errors=0;
        traceBuf = false;
        printToks = false;
    }
    void initWithBuf(const char*buf);
    void initWithFile(const char*filename);
    
    Toks *parseFile(const char* filename);
    Toks *parseBuf(const char* buf);
    Toks *parseBufTrace(const char* buf, const char *file);
    
    void addPar(Toks*toks);
    int addList(Par*par,Toks*toks,int toki);
 
    void bindName(Par*par);
    void promoteOnlyChild(Par *par);
    void bindParTree(Par*par);
    void bindGrammar();
};