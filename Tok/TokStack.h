/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "Tok.h"
#import <unordered_map>

struct TokLevel {
    
    int toki;
    int level;
    
    TokLevel(int toki_, int level_) {
        toki = toki_; //TODO: refactor as toki -- is there a conflict?
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
    int getToki();
    int getLevel();
};
