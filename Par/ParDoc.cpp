#import "ParDoc.h"
#import "stdlib.h"
#import "math.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"  

int ParDoc::nextDocId = 1000;

/* move idx to beginning of next word
 */
bool ParDoc::nextWord() {
    
    for (;idx<size; idx++) {
        
        switch (_chr[idx]) {
                
            case '\t': 
            case '\n': 
            case '\r': 
            case  ' ':
                
                eatWhitespace();
                return (idx<size);
                
            case '\0':
                
                return false;
                
            default:
                
                continue;
        }
    }
        return false;
}

/* point to start of whitespaces preceeding _chr[i]
 * for "a  b" 
 * i:3 'b'  => i:1 ' '
 * i:4 '\0' => i:4
 */
int ParDoc::trimSpace(int i) {
    
    if (_chr[i]=='\0')
        return i;
    
     for (; i>0; i--) {
        
        switch (_chr[i-1]) {
                
            case '\t':
            case '\n':
            case '\r':
            case  ' ': continue;
                
            default:
            case '\0': return i;
        }
    }
    return 0;
}

/* trim trailing whitespace charactires from end of doc
 */
void ParDoc::trimTrailspace() {

    for (; size>0; size--) {
        
        switch (_chr[size-1]) {
                
            case '\t':
            case '\n':
            case '\r':
            case  ' ':
                
                continue;
                
            default:
                
                _chr[size]='\0';
                return;
        }
    }
 }
/* move idx forward to first non-whitespace character
 */
void ParDoc::eatWhitespace() {
    
    for (;idx<size; idx++) {
        
        switch (_chr[idx]) {
                
            case '\t': 
            case '\n': 
            case '\r': 
            case  ' ':

                break;
                
            default:
            case '\0':
                
                return;
        }
    }
}
/* current cursor is not at end of doc 
 */
bool ParDoc::hasMore() {
    
    if  ((_chr[idx]!= '\0') &&
         (idx < size)) {
        return true;
    }
    else {
        return false;
    }
}
