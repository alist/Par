#import <vector>

struct ParDoc {
    
    static std::vector<ParDoc*> Stack;
    static ParDoc Deepest;
 
    char*chr;
    int row;
    int col;
    int size;
    
    ParDoc() {
        
        chr = 0;
        row = 0;
        col = 0;
        size = 0;
    }

    ParDoc(char*char_){
        
        ParDoc();
        chr = char_;
        Deepest = *this;
    }
    
    void operator = (ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    void eatWhitespace();
};