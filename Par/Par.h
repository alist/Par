#import "ParQuo.h"
#import "ParRegx.h"
#import "ParDoc.h"
#import "TokNameEnum.h"

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
} MatchType;

struct Par;
typedef vector<Par*> ParList;

struct Par {
    
    string name;
    RepeatType repeat;
    MatchType matching;
    ParList parList;
    
    union {
        ParRegx *regx;
        ParQuo *quo;
    } match;
    
    static bool Trace; // trace the parse as it happens
    static int MemoNow;
    int memoMe; //memoize TODO: only for printing?
 
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
    
    bool parse    (Toks*, ParDoc &input, int level);
    
    bool parseOne (Toks*, ParDoc &input, int level);
    bool parseMny (Toks*, ParDoc &input, int level);
    bool parseAny (Toks*, ParDoc &input, int level);
    bool parseOpt (Toks*, ParDoc &input, int level);
    
    bool parseAnd (Toks*, ParDoc &input, int level);
    bool parseOr  (Toks*, ParDoc &input, int level);
    bool parseQuo (Toks*, ParDoc &input, int level);
    bool parseRegx(Toks*, ParDoc &input, int level);

    void parseBufToFile (const char*buf, const char*traceFile, bool openStderr);
    void parseFileToFile (const char*inputFile, const char*tracefile);
    static char *readFile (const char*inputFile);
  };
