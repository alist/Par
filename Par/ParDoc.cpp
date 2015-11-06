#import "ParDoc.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"  

int ParDoc::nextDocId = 1000;

void ParDoc::operator =(ParDoc&p_) {
 
    docId = nextDocId++;
    _chr = p_._chr;
    idx  = p_.idx;
    row  = p_.row;
    col  = p_.col;
    size = p_.size;

}

bool ParDoc::operator == (ParDoc&p_) {
    
    if (row == p_.row &&
        col == p_.col)
        return true;
    else return false;
}

void ParDoc::operator += (int offset) {
    
    for (int i=0; i<offset; i++, idx++) {
        
        switch (_chr[idx]) {
                
            case '\0': return;
            case '\t': col = (col|0x3)+1; break; // for tabs==4 chr
            case '\n': col = 0; row++; break;
            case '\r': break;
            default: col++; break;
        }
    }
}


bool ParDoc::nextWord() {
    
    for (;idx<size; idx++) {
        
        if (_chr[idx]== ' ') {
            
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
            case '\t': col = (col|0x3)+1; break; // for tabs==4 chr
            case '\n': col = 0; row++; break;
            case '\r': break;
            case  ' ': col++; break;
            default: return;
        }
    }
}

bool ParDoc::hasMore() {
    if  (idx < size-1) {
        return true;
    }
    else {
        return false;
    }
}
