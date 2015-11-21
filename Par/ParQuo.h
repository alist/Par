/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

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
    virtual bool parse(ParDoc &doc) {
        
        if (strncmp(doc.ptr(),pattern,size)==0) {
            doc += size;
            doc.eatWhitespace();
            return true;
        }
        return false;
    }    
    
    operator const char *() { return pattern;}
};
