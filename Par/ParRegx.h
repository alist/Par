#import <locale.h>
#import <stdlib.h> 
#import <string>
#import "ParDoc.h"

/* PCRE is not standard on OS X. So, install local version to this product in ./pcre
 but only works for version 10.6 otherwise need to download and make a new version:
  1) download the zip file from http://pcre.org/ and unzip to a temp directory
  2) within the temp directory, excute: "./configure; make; make install" 
  3) cp /usr/local/include/pcre* pcre/include
  4) cp /usr/local/lib/libpcre* pcre/lib
 */

#import "pcre.h"

using namespace std;

struct ParRegx  {
    
    bool patternOk;
    char * pattern;

    string result;
    int size;
    bool found;
    
    pcre *re;
    const char*error;
    int erroffset;
    #define OveCount 30 // multiple of three
    int ovector[OveCount];
    int rc;
    
    ParRegx(const char*pattern_) {
        compile (pattern_);
        size  = 0;
        found = false;
    }
    ParRegx() {
        
        pattern = 0;
        patternOk = false;
        size  = 0;
        found = false;
    }
    ~ParRegx() {

        if (re) {
            pcre_free(re);
        }
    }
    
    bool compile(const char*pattern_) {
        
        if (pattern_) {

            pattern = (char*)pattern_;
            re = pcre_compile (pattern, 0 ,&error, &erroffset,0);
            if (re) {
                
                patternOk = true;
                return true;
            }
        }
        patternOk = false;
        return false;
    }
    
    bool match(char* input) {
    
        if (re) {
            rc = pcre_exec(re,0,input,(int)strlen(input),0,0,ovector,OveCount);
            if (rc>0) // matched all subgroups
                return true;
        }
        return false;
    }
    virtual bool parse(ParDoc &input_, char *pattern_=0) {
        
        //??? input_.eatWhitespace();
        
        if (pattern_) {
            compile (pattern_);
        } else {
            compile (pattern);
        }
        if (patternOk && match(input_._char)) {
            found = true;
        } 
        else {
            pcre_free(re);
            return false;
        }

        if (rc>1) {
            
            size = ovector[3] - ovector[2]; // size if submatch inside first parens, when available
            
            if (size>0) {
                result.assign(input_._char + ovector[2], size);
                input_ += ovector[1]; // end of total parse
                input_.eatWhitespace();
                size = ovector[1] - ovector[0];
                pcre_free(re);
                return true;
            }
        }
        //TODO: is this needed?
        if (rc>0) {

            size = ovector[1] - ovector[0];
            if (size>0) {
                result.assign(input_._char + ovector[0], size);
                input_ += ovector[1]; // end of total parse
                input_.eatWhitespace();
                pcre_free(re);
                return true;
            }
        }
        pcre_free(re);
        return false;
    }    
    
    void eval() {}
    operator int() { return (int)atoi(result.c_str()); }    
    operator float() { return (float)atof(result.c_str()); }    
    operator const char *() { return result.c_str(); }
};
