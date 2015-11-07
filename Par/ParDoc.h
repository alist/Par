#import <vector>
#import <stdlib.h>
#import <string.h>

struct ParDoc {
    
    char*_chr;
    
    int front; // front of last match;
    int idx;  // cursor after last match
    int size;
    int docId;
    
    static int nextDocId;
    
    ParDoc() {
        
        docId = nextDocId++;
        _chr = 0;
        size = 0;
        front = 0;
    }

    ParDoc(char*chr_){
        
        docId = nextDocId++;
        _chr = chr_;
        idx = 0;
        front = 0;
        size = (int)strlen(_chr);

    }
    void frontBack(int,int);
    
    // create substring by forcing a terminating char
    char pushCutHack(int cutIdx) {
 
        if (cutIdx>size-1) {
            return '\0';
        }
        else {
            char hack =_chr[cutIdx];
            _chr[cutIdx] = '\0';
            return hack;
        }
    }
    // restore from substring
    void popCutHack(int cutIdx,char hack) {
        
        if (cutIdx<size) {
            _chr[cutIdx] = hack;
        }
    }
    
    char *ptr() {
        return _chr+idx;
    }
    
    bool nextWord();
    bool hasMore();
    void operator = (ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    int trimSpace(int i);
    void eatWhitespace();
};