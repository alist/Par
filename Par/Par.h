#import "ParQuo.h"
#import "ParRegx.h"
#import "ParDoc.h"
#import "TokTypes.h"

#import <vector>
#import <iterator>
#import <string>
#import <stdarg.h>
#import <stdio.h>

typedef enum {
    kRepOne,
    kRepMny,
    kRepAny,
    kRepOpt,
} RepeatType;


typedef enum {
    kMatchRegx,
    kMatchQuo,
    kMatchAnd,
    kMatchOr,
    kMatchMeta,
} MatchType;

struct Par;
typedef vector<Par*> ParList;

/* some matches succeed without creating a token such as a? and a*, 
 * in which case a preceeding pattern may consume tokens
 * such as b in (a ~b c?), or (a ~b c*) that will consume all the words
 * leading up to end of a line or document.
 * So,
 */
typedef enum {
    kRetNope  = 1<<0, // zero matches for a a+
    kRetZero  = 1<<1, // zero matches for a? a*
    kRetMatch = 1<<2, // one or more matches for a a+ a? a*
    kRetEnd   = 1<<3  // end of file
} RetFlag;


struct Par {
    
    string name;
    RepeatType repeat;
    MatchType matching;
    ParList parList;
    int maxCount;
    
    union {
        ParRegx *regx;
        ParQuo *quo;
    } match;
    
    static bool Trace; // trace the parse as it happens
    static int MemoNow;
    int memoMe; //memoize
    static int MaxCountDefault; // max matches for a* or a+
    static int MaxLevelDefault; // max levels deep 
 
    Par(){init();};
    
    Par(RepeatType t){init(t);}
    Par(MatchType t){init(t);}
    Par(string*);
    Par(Par*par);
    Par(ParQuo*quo) {init(quo);}
    Par(ParRegx*rx) {init(rx);}
    
    void init();
    void init(RepeatType);
    void init(MatchType);
    void init(ParQuo*quo);
    void init(ParRegx*rx);
    
    void add(Par*n);

    void setName(const char*who_);
    
    Par& operator = (Par&p_) ;
    Par& operator [](int index);
    
    operator float();    
    operator int();    
    operator const char*();
    
    void printLevelIndent(int level);
    void printLevelInputMargin(int level, ParDoc&input);
    
    int  pushTok  (Toks*, int level, ParDoc&input);
    void popTok   (Toks*, int tokenSizeBefore);
    
    RetFlag parse    (Toks*, ParDoc &input, int level);
    
    RetFlag parseOne (Toks*, ParDoc &input, int level);
    RetFlag parseMny (Toks*, ParDoc &input, int level);
    RetFlag parseAny (Toks*, ParDoc &input, int level);
    RetFlag parseOpt (Toks*, ParDoc &input, int level);
    
    RetFlag parseAnd (Toks*, ParDoc &input, int level);
    RetFlag parseOr  (Toks*, ParDoc &input, int level);
    RetFlag parseQuo (Toks*, ParDoc &input, int level);
    RetFlag parseRegx(Toks*, ParDoc &input, int level);

    void parseBufToFile (const char*buf, const char*traceFile, bool openStderr);
    void parseFileToFile (const char*inputFile, const char*tracefile);
  };
