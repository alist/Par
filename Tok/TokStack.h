#import "Tok.h"
#import <unordered_map>

struct ParTokLevel {
    
    int tokt;
    int level;
    
    ParTokLevel(int tokt_, int level_) {
        tokt = tokt_;
        level = level_;
    }
};

// see TokStack.doc

struct TokStack {
    
    vector<ParTokLevel*> stack;
    unordered_map<int,int> mamaTokt; // toki to parent toki mapping
    unordered_map<int,int> levelMama; // at current level, who's yo mama
    
    TokStack();
    void setStack(int level);
    void setStack(int level, int toki);
    int getTokt();
    int getLevel();
};
