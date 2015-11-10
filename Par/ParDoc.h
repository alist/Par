#import <vector>
#import <stdlib.h>
#import <string.h>
/* ParDoc saves state on a shared char* _chr ; many ParDocs may point to the same _chr.
 *
 */
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
    
    /*      *** pushCutHack is NOT THREAD SAFE ***
     *
     * hack an in-place substring by overwriting a '\0' at beginning of last match.
     * The hack char is managed by the caller, which also copies the state, like so:
     *
     *      ParDoc cut = doc; // does not copy _chr
     *      char hack = cut.pushCutHack();
     *      ... 
     *      cut.popCutHack(hack);
     *      doc = cut;
     *
     *      *** This is NOT THREAD SAFE ***
     *      used for recursive parsing where the substrings never overlap
     */
    char pushCutHack() {
 
        if (front>size-1) {
            return '\0';
        }
        else {
            char hack =_chr[front];
            _chr[front] = '\0';
            return hack;
        }
    }
    /* restore character - see pushCutHack for details
     */
    void popCutHack(char hack) {
        
        if (front<size) {
            _chr[front] = hack;
        }
    }
    
    char *ptr() {
        return _chr+idx;
    }
    
    bool nextWord();
    bool hasMore();
    void copyState(ParDoc&p_);
    bool operator == (ParDoc&p_);
    void operator += (int offset);
    int trimSpace(int i);
    void eatWhitespace();
};