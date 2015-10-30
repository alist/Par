#import "TokNameEnum.h"
#import "Par.h"

struct ParTok {
    
    Toks *tokens;
    Par *root;

    ParTok(Par*root_) {tokens = new Toks(); root = root_;}
    ParTok()          {tokens = new Toks();}
    
    ~ParTok(){deleteToks();}
    
    void deleteToks();
    void printToks();
    
    virtual void initToks(Par*);
    virtual void parseBuf(const char*buf, bool trace, bool print);
    
};