#import "ParDoc.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"  

std::vector<ParDoc*> ParDoc::Stack;
ParDoc ParDoc::Deepest;
int ParDoc::nextDocId = 1000;

void ParDoc::operator =(ParDoc&p_) {
    
    chr   = p_.chr;
    row   = p_.row;
    col   = p_.col;
    size  = p_.size;
    
    if (Deepest.row < row ||
        (Deepest.row == row && Deepest.col < col)) {
        
        Deepest.row = row;
        Deepest.col = col;
        Deepest.chr = chr;
    }
    
    while (Stack.size() > size) {
        
        ParDoc*crc = Stack.back();
        Stack.pop_back();
        delete (crc);
    }
}

bool ParDoc::operator == (ParDoc&p_) {
    
    if (row == p_.row &&
        col == p_.col)
        return true;
    else return false;
}
void ParDoc::operator +=(int offset) {
    
    for (int i=0; i<offset; i++, chr++) {
        
        switch (*chr) {
                
            case '\0': return;
            case '\t': col = (col | 0x3)+1; break; // for tabs==4 chr
            case '\n': col = 0; row++; break;
            case '\r': break;
            default: col++; break;
        }
    }
}
void ParDoc::eatWhitespace() {
    
    for (;*chr != '\0'; chr++) {
        
        switch (*chr) {
                
            case '\0': return;
            case '\t': col = (col | 0x3)+1; break; // for tabs==4 chr
            case '\n': col = 0; row++; break;
            case '\r': break;
            case  ' ': col++; break;
            default: return;
        }
    }
}
