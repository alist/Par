#import "ParTok.h"
#import <unordered_map>

struct ParTokLevel {
    int tokt;
    int level;
    
    ParTokLevel(int tokt_, int level_) {
        tokt = tokt_;
        level = level_;
    }
};

// see ParTokStack.doc

struct ParTokStack {
    
    vector<ParTokLevel*> stack;
    unordered_map<int, int> mamaTokt; // toki to parnet toki mapping
    unordered_map<int, int> levelMama; // at current level, who's yo mama
    
    ParTokStack();
    void setStack(int level);
    void setStack(int level, int toki);
    int getTokt();
    int getLevel();
};
