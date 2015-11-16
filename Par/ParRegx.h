#import <locale.h>
#import <stdlib.h> 
#import <string>
#import "ParDoc.h"

/* PCRE is not standard on OS X. So, install local version to this product in ./pcre
 To update:
  1) download the zip file from http://pcre.org/ and unzip to a temp directory
  2) within the temp directory, excute: "./configure; make; make install" 
  3) cp /usr/local/include/pcre* pcre/include
  4) cp /usr/local/lib/libpcre* pcre/lib
 */

//TODO: NFA vs. DFAs https://swtch.com/~rsc/regexp/regexp1.html


#import "pcre.h"

using namespace std;

struct ParRegx  {
    
    string result;  // result of match
    string result2; // alternate return value (^'one'=>'1')
    int size;
    bool found;
    
    pcre *re;
    int rc;
   
    bool advanceDoc; // advance doc pointer for a match

    /* the next three are only used if advanceDoc==false
     * to prevent repeating matches on the same doc 
     * for example: 
     *
     *   def (hello)+
     *   hello (n'hello')
     */
     
    int matchDocId=0;
    
#define OveCount 30 // multiple of three
    int ovector[OveCount];
    
    ParRegx(const char*pattern) {
        
        re = 0;
        size  = 0;
        found = false;
        advanceDoc = true;

        /* test for options:
         * i'... case insensitive match
         * n'... do not advance cursor to end of match
         *
         * not the macro Regx("^([in]*\'[^\']*)\'"))
         * returns only the initial option letters and leading single quote '
         */
        int maxOpt = 2; // maximum number of options
        int pcreOptions = 0;
        const char *opt = pattern;
        for (int i=0; i<=maxOpt; i++, opt++) {
            switch (*opt) {
                case '\\': // avoid preceeding slash in ('\'...'), so drop thru
                case '\0': pcreOptions = 0; goto done; // no trailing ' so not an (i'...'), et
                case '\'': pattern = opt+1; goto done;   // found (i',(n',(ni', etc
                case 'i': pcreOptions |= PCRE_CASELESS; break;
                case 'n': advanceDoc = false; break;
                default: break;
            }
        }
    done:
        /* compile the pattern */
        
        const char*error=0;
        int erroffset=0;
        if (strlen(pattern)>0) {
            re = pcre_compile (pattern, pcreOptions ,&error, &erroffset,0);
        }
        if (error) {
            
            fprintf(stdout, "\n*** error: %s",error);
            fprintf(stdout, "%s\n",pattern);
            
            for (int i=0;i<erroffset-1; i++) {
                fprintf(stdout, " ");
            }
            fprintf(stdout, "^");
        }
    }
    bool match(char* pattern) {
        
        if (re) {
            rc = pcre_exec(re,0,pattern,(int)strlen(pattern),0,0,ovector,OveCount);
            if (rc>0) // matched all subgroups
                return true;
        }
        return false;
    }
    virtual bool parse(ParDoc &doc) {
        
        // only match once (n'meet')
        if (advanceDoc==false && matchDocId==doc.docId) {
            return false;
        }
        if (re && match(doc.ptr())) {
            
            matchDocId = doc.docId;
            found = true;
        } 
        else {
            return false;
        }

        // submatch inside first parens ('(m(ee)?t)')
        
        if (rc>1) {
            
            size = ovector[3] - ovector[2];
            
            if (size>0) {
                
                // ('(m(ee)?t)'=>'meet') // "mt" => "meet"
                if (result2.size()) {
                    result = result2;
                }
                // ('(m(ee)?t)') // "mt" => "mt"
                else {
                     result.assign(doc.ptr() + ovector[2], size);
                }
                if (advanceDoc) {
                    doc += ovector[1];
                    doc.eatWhitespace();
                }
                size = ovector[1] - ovector[0];
                return true;
            }
        }
    
        // no enclosing parens  ('one')
    
        if (rc==1) {

            size = ovector[1] - ovector[0];
            if (size>0) {
                
                // ('one'=>'1') // "one" => "1"
                if (result2.size()) {
                    result = result2;
                }
                 // ('one') // "one" => "one"
                else {
                    result.assign(doc.ptr() + ovector[0], size);
                }
                if (advanceDoc) {
                    doc+= ovector[1];
                    doc.eatWhitespace();
                }
                return true;
            }
        }
        return false;
    }    
    
    void eval() {}
    operator int() { return (int)atoi(result.c_str()); }
    operator float() { return (float)atof(result.c_str()); }    
    operator const char *() { return result.c_str(); }
};
