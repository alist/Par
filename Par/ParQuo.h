#import <locale.h>
#import <stdlib.h> 
#import <string.h>
#import "ParDoc.h"

using namespace std;

struct ParQuo {
    
    char *pattern;
    int size;
    bool found;
     
    ParQuo(const char*pattern_) {
        
        size = (int)strlen(pattern_);
        pattern = new char[size + 1];
        strcpy(pattern, pattern_ );
        found = false;
    }
    ~ParQuo() {
        if (pattern) {
            delete pattern;
        }
    }
    virtual bool parse(ParDoc &input) {
        
        if (strncmp(input.chr,pattern,size)==0) {
            
            input += (int)size;
            input.eatWhitespace();
            return true;
        }
        return false;
    }    
    
    void eval() {}
    operator int() { return (int)atoi(pattern); }    
    operator float() { return (float)atof(pattern); }    
    operator const char *() { return pattern;}
};
