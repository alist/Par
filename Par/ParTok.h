#import "TokNameEnum.h"

struct ParTok {
    
    Toks *tokens;

    ParTok(){tokens=0;}
    ~ParTok(){deleteToks();}
    
    void deleteToks();
    void printToks();
    
    virtual void initToks()=0;
    virtual void parseBuf(const char*buf, bool print)=0;
    
};