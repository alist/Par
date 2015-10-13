#import <locale.h>
#import <stdlib.h> 
#import <string.h>
#import "ParDoc.h"

using namespace std;

struct ParQuo {
    
    const char *_pattern;
    int _size;
    bool _found;
     
    ParQuo(const char*pattern_) {
        
        _pattern = (char*)pattern_;
        _size = (int)strlen(_pattern);
        _found = false;
    }
    ParQuo() {
        _pattern = 0;
        _found = false;
    }
    ~ParQuo() { }
    virtual bool parse(ParDoc &input_, char *pattern_=0) {
        
        if (strncmp(input_._char,_pattern,_size)==0) {
            
            input_ += (int)_size;
            input_.eatWhitespace();
            return true;
        }
        return false;
    }    
    
    void eval() {}
    operator int() { return (int)atoi(_pattern); }    
    operator float() { return (float)atof(_pattern); }    
    operator const char *() { return _pattern;}
};
