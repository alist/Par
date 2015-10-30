#import <vector>

struct ParDoc {
    
    static std::vector<ParDoc*> _stack;
    static ParDoc _deepest;
 
    char*_char;
    int _row;
    int _col;
    int _size;
    
    ParDoc() {
        
        _char = 0;
        _row = 0;
        _col = 0;
        _size = 0;
    }

    ParDoc(char*char_){
        
        ParDoc();
        _char = char_;
        _deepest = *this;
    }
    
    void operator = (ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    void eatWhitespace();
};