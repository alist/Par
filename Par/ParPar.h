/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "Tok.h"
#import "Par.h"
#import "ParParToks.h"
#import <string>
#import <unordered_map>

typedef unordered_map<std::string,Par*>NamePars;

struct ParToks;
/* create a parser for .par files, steps:
 * 1) read the .boot file and invoke
 */
struct ParPar {
    
    ParParToks parParToks;
    NamePars namePars;  // namePars[*name] = par
    ParList grammar;    // list of Pars that refer to each other
    ParToks *parToks;   // invoke grammar on input buffer to produce steam of tokens
    bool traceDoc;      // option to show sliding window of input buffer while tokenizing
    bool printToks;     // option to print resulting tokenstream
    int errors;         // catch all error
    
    ParPar() {
        
        errors=0;
        traceDoc = false;
        printToks = false;
    }
    void readGrammar(const char*filename, FILE *fp);
    void buf2Grammar(const char*buf, FILE *fp);
    
    Toks *file2Toks(const char* filename,FILE *fp);
    Toks *buf2Toks(const char* buf, FILE *fp);
    Toks *parseBuf2File(const char* buf, const char *file);
    
    void toks2Grammar(Toks*toks);
    int addList(Par*par,Toks*toks,int toki);
 
    void bindName(Par*par);
    void promoteOnlyChild(Par *par);
    void bindParTree(Par*par);
    void bindGrammar();
};