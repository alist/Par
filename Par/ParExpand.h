
#import "ParTok.h"
#import "ParTokStack.h"
#import <unordered_map>

struct ParExpand {
    
    ParToks *tokens;
    ParTokStack tokStack;
    
    ParExpand() {}
    
    void expandTokens(ParToks*tokens_);
    bool thisTokIsSibling(int toki, int level);
    ParTok *incrementToki(int &toki);
    int expand(int toki);
    //int expandMany (TypeTr3ss *ttss, int toki);
};