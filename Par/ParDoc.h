#import <vector>

struct ParDoc {
    
    char*_chr;
    int row;
    int col;
    int size;
    int idx;
    int docId;
    
    static int nextDocId;
    
    ParDoc() {
        
        docId = nextDocId++;
        _chr = 0;
        row = 0;
        col = 0;
        size = 0;
    }

    ParDoc(char*chr_){
        
        docId = nextDocId++;
        _chr = chr_;
        row = 0;
        col = 0;
        idx = 0;
        size = (int)strlen(_chr);
    }
    char*ptr() {return _chr+idx;}
    bool nextWord();
    bool hasMore();
    void operator = (ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    int trimSpace(int i);
    void eatWhitespace();
};