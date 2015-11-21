/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import <vector>
#import <stdlib.h>
#import <string.h>

/* ParDoc is a shared char* on _chr -- ParDoc is SINGLE THREADED -- see below
 * Many - many ParDocs may point to the same _chr, which is useful for backtracking.
 * As the Par matches words in grammar, it advances the cursor and adds another ParDoc 
 * on the stack. For a non-match, Par backtracks pops prior versions of ParDoc off the stack.
 * Because all ParDocs point to the samem _chr, there are no copies needed.
 *
 * There is a special case for lookahead parsing; for example: cd in
        
    a(b ~cd e)
    cd (c d) 
    c('c') 
    d('d') 
    e('e')
 
 * where ~cd needs to match 
 
    "b c d e" // "c d" occuring before "e"
 
 * but not match 
 
    "b e c d" // "c d" occuring after "e"
 
 * There are two approaches: slow and fast.
 * Slow approach is to create a new string "c d" and delete after subparse;
 * Fast approach is to terminate before lookahead match: "b c d" + '\0' + "e"
 * Given the overhead for new() and free(), a fast single-thread probably
 * outperforms a slow multi-thread for most grammars and documents, so
 * ParDoc uses the fast approach. This, ParDoc is NOT THREADSAFE for 
 * lookahead (only). See pushCutHack and popCutHack for details.
 *
 * Island parsing is not affected, because the unmatched portion of _chr
 * is copied to a newr "unmatched" token and then handled after tokenizing.
 */
class ParDoc {
    
    char*_chr;
    
public:
    
    int idx;  // index of cursor after last match
    int size; // total size to beginning of terminating whitespace in _chr
    int docId;
    
    static int nextDocId;
    
    ParDoc() {
        
        docId = nextDocId++;
        _chr = 0;
        size = 0;
    }

    ParDoc(char*chr_){
        
        docId = nextDocId++;
        _chr = chr_;
        idx = 0;
        size = (int)strlen(_chr);

    }
    
    ParDoc(ParDoc&p_) {
     
        docId = nextDocId++;
        _chr = p_._chr;
        idx  = p_.idx;
        size = p_.size;
    }
    
    ParDoc& operator = (ParDoc&p_)  {
        
        _chr = p_._chr;
        idx  = p_.idx;
        size = p_.size;
        return *this;
    }
    
     void operator += (int offset) {
         idx = std::min(idx+offset,size);
    }

    
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
    char pushCutHack(int endIdx) {
 
        if (endIdx>size-1) {
            return '\0';
        }
        else {
            char hack =_chr[endIdx];
            _chr[endIdx] = '\0';
            return hack;
        }
    }
    /* restore character - see pushCutHack for details
     */
    void popCutHack(int endIdx,char hack) {
        
        if (endIdx<size) {
            _chr[endIdx] = hack;
        }
    }
    char *buf() {
        return _chr;
    }
    char *ptr() {
        return _chr+idx;
    }
    
    bool nextWord();
    bool hasMore();
    int trimSpace(int i);
    void trimTrailspace();
    void eatWhitespace();
};