#import "ParTokStack.h"


ParTokStack::ParTokStack() {
    levelMama[0]=0;
    mamaTokt[0]=0;
}

void ParTokStack::setStack(int level) {
    
    ParTokLevel *back = stack.back();
    
    if (back->level >= level) {
        
        // pop all sibling/child tokens (a>=b)
        for (;back && back->level >= level; back = stack.back()) {
            stack.pop_back();
            delete back;
        }
    }
}
void ParTokStack::setStack(int level, int toki) {
    
    int oldLevel         = (stack.size() ? stack.back()->level : 0);
    ParTokLevel *oldBack = (stack.size() ? stack.back() : 0);
    
    // pop all child tokens (a>b)
    while (stack.size() && stack.back()->level>level) {
        
        ParTokLevel *back = stack.back();
        stack.pop_back();
        delete back;
    }
    if (level > oldLevel) {
        levelMama[level]= (oldBack ? oldBack->tokt : 0);
    }
    ParTokLevel *newBack = new ParTokLevel(toki,level);
    stack.push_back(newBack);
    mamaTokt[toki] = levelMama[level];
}

int ParTokStack::getTokt() {
    if (stack.size()==0) {
        return 0;
    }
    else {
        return stack.back()->tokt;
    }
}
int ParTokStack::getLevel() {
    if (stack.size()==0) {
        return 0;
    }
    else {
        return stack.back()->level;
    }
}


