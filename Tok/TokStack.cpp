/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "TokStack.h"

TokStack::TokStack() {
    
    levelParent[0]=0;
    tokiParent[0]=0;
}

void TokStack::setStack(int level) {
    
    TokLevel *back = stack.back();
    
    if (back->level >= level) {
        
        // pop all sibling/child tokens (a>=b)
        for (;back && back->level >= level; back = stack.back()) {
            stack.pop_back();
            delete back;
        }
    }
}
/* if going shallower, then pop deeper tokens
 * if going deeper, then push new token
 * it's possible for the stack has 2 tokens of the same level
 */
void TokStack::setStack(int level, int toki) {
    
    int oldLevel = (stack.size() ? stack.back()->level : 0);
    TokLevel *oldTokLevel = (stack.size() ? stack.back() : 0);
    
    // pop all child tokens (a>b)
    while (stack.size() && stack.back()->level>level) {
        
        TokLevel *back = stack.back();
        stack.pop_back();
        delete back;
    }
    if (level > oldLevel) {
        levelParent[level] = (oldTokLevel ? oldTokLevel->toki : 0);
    }
    TokLevel *newBack = new TokLevel(toki,level);
    stack.push_back(newBack);
    tokiParent[toki] = levelParent[level];
}

int TokStack::getToki() {
    
    if (stack.size()==0) {
        return 0;
    }
    else {
        return stack.back()->toki;
    }
}

int TokStack::getLevel() {
    
    if (stack.size()==0) {
        return 0;
    }
    else {
        return stack.back()->level;
    }
}


