#import "Tok.h"
#import <unordered_map>

struct TokLevel {
    
    int tokt;
    int level;
    
    TokLevel(int toki_, int level_) {
        tokt = toki_; //TODO: refactor as toki -- is there a conflict?
        level = level_;
    }
};

// see TokStack.doc

struct TokStack {
    
    vector<TokLevel*> stack;
    unordered_map<int,int> tokiParent; // toki to parent toki mapping
    unordered_map<int,int> levelParent; // at current level, who's yo mama
    
    TokStack();
    void setStack(int level);
    void setStack(int level, int toki);
    int getTokt();
    int getLevel();
};
