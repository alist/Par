#import <vector>

struct ParDoc {
    
    static std::vector<ParDoc*> Stack;
    //static ParDoc Deepest;
 
    char*chr;
    int row;
    int col;
    int size;
    int docId;
    static int nextDocId;
    
    ParDoc() {
        
        docId = nextDocId++;
        chr = 0;
        row = 0;
        col = 0;
        size = 0;
    }

    ParDoc(char*char_){
        
        docId = nextDocId++;
        chr = char_;
        row = 0;
        col = 0;
        size = (int)strlen(chr);
        //Deepest = *this;
    }
    
    void operator = (ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    void eatWhitespace();
};