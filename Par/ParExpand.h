
#import "Tok.h"
#import "TokStack.h"
#import <unordered_map>

struct ParExpand {
    
    Toks *tokens;
    TokStack tokStack;
    
    ParExpand() {}
    
    void expandTokens(Toks*tokens_);
    bool thisTokIsSibling(int toki, int level);
    Tok *incrementToki(int &toki);
    int expand(int toki);
    //int expandMany (TypeTr3ss *ttss, int toki);
};