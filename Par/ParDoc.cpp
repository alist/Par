#import "ParDoc.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"  

std::vector<ParDoc*> ParDoc::_stack;
ParDoc ParDoc::_deepest;

void ParDoc::operator =(ParDoc&p_) {
    
    _char  = p_._char;
    _row   = p_._row;
    _col   = p_._col;
    _size  = p_._size;
    
    if (_deepest._row < _row ||
        (_deepest._row == _row && _deepest._col < _col)) {
        
        _deepest._row = _row;
        _deepest._col = _col;
        _deepest._char = _char;
    }
    
    while (_stack.size() > _size) {
        
        ParDoc*crc = _stack.back();
        _stack.pop_back();
        delete (crc);
    }
}

bool ParDoc::operator ==(ParDoc&p_) {
    
    if (_row == p_._row &&
        _col == p_._col)
        return true;
    else return false;
}
void ParDoc::operator +=(int offset) {
    
    for (int i=0; i<offset; i++, _char++) {
        
        switch (*_char) {
                
            case '\0': return;
            case '\t': _col = (_col | 0x3)+1; break; // for tabs==4 char
            case '\n': _col = 0; _row++; break;
            case '\r': break;
            default: _col++; break;
        }
    }
}
void ParDoc::eatWhitespace() {
    
    for (;*_char != '\0'; _char++) {
        
        switch (*_char) {
                
            case '\0': return;
            case '\t': _col = (_col | 0x3)+1; break; // for tabs==4 char
            case '\n': _col = 0; _row++; break;
            case '\r': break;
            case  ' ': _col++; break;
            default: return;
        }
    }
}
