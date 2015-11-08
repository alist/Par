#import "ParDoc.h"
#import "stdlib.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"  


int ParDoc::nextDocId = 1000;

/* this is used for backtracking, so does not own string
 */

void ParDoc::copyState(ParDoc&p_) {
    
    docId = nextDocId++;
    _chr = p_._chr;
    idx  = p_.idx;
    size = p_.size;
    front = p_.front;
}

bool ParDoc::operator == (ParDoc&p_) {
    
    if (idx == p_.idx) {
        return true;
    }
    else {
        return false;
    }
}

void ParDoc::operator += (int offset) {
    
    idx = std::min(idx+offset,size-1);
    
}
void ParDoc::frontBack(int frontOffset, int backOffset) {

    front = std::min(idx+frontOffset,size);
    idx   = std::min(idx+backOffset,size);
}

bool ParDoc::nextWord() {
    
    for (;idx<size; idx++) {
        
        if (_chr[idx]==' ') {
            
            break;
        }
    }
    eatWhitespace();
    return (idx <size);
}
int ParDoc::trimSpace(int i) {
    
    for (;i>0; i--) {
        
        if (_chr[i]!= ' ') {
            
            return i;
        }
    }
    return 0;
}

void ParDoc::eatWhitespace() {
    
    for (;idx<size-1; idx++) {
        
        switch (_chr[idx]) {
                
            case '\0': return;
            case '\t': break; 
            case '\n': break;
            case '\r': break;
            case  ' ': break;
            default: return;
        }
    }
}

bool ParDoc::hasMore() {
    
    if  ((_chr[idx]!= '\0') &&
         (idx < size-1)) {
        return true;
    }
    else {
        return false;
    }
}
