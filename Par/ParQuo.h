#import <locale.h>
#import <stdlib.h> 
#import <string.h>
#import "ParDoc.h"

using namespace std;

struct ParQuo {
    
    const char *pattern;
    int size;
    bool found;
     
    ParQuo(const char*pattern_) {
        
        pattern = (char*)pattern_;
        size = (int)strlen(pattern);
        found = false;
    }
    ParQuo() {
        pattern = 0;
        found = false;
    }
    ~ParQuo() { }
    virtual bool parse(ParDoc &input_, char *pattern_=0) {
        
        if (strncmp(input_.chr,pattern,size)==0) {
            
            input_ += (int)size;
            input_.eatWhitespace();
            return true;
        }
        return false;
    }    
    
    void eval() {}
    operator int() { return (int)atoi(pattern); }    
    operator float() { return (float)atof(pattern); }    
    operator const char *() { return pattern;}
};
