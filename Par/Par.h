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
    kRepOne,    // a
    kRepMny,    // a+
    kRepAny,    // a*
    kRepOpt,    // a?
} RepeatType;

typedef enum {
    kMatchRegx,  // 'a'
    kMatchQuo,   // "a"
    kMatchAnd,   // (a b)
    kMatchOr,    // (a | b)
    kMatchAhead, // (~a b)
    kMatchWave,  // (a ~ b)
} MatchType;


/* The grammar "a (b c)" contains two main parts
 * name:    "a"
 * parList: ["b", "c"]
 */
struct Par;
typedef vector<Par*> ParList;

struct Par {
    
    string name;
    MatchType matching;
    ParList parList;
    int maxCount;
    int minCount;
    
    union {
        ParRegx *regx;
        ParQuo  *quo;
    } match;
    
    static bool Trace; // trace the parse as it happens
    static int MemoNow;
    int memoMe; //memoize
    static int MaxCountDefault; // max matches for a* or a+
    static int MaxLevelDefault; // max levels deep 
 
    /* Used only by ParMacro Par_() to bootstrap the parser */
    
    Par(){init();};
    void setName(const char*who_);
    void init(RepeatType);
    
    /* Used while parsing *.def files */
    
    Par(MatchType t){init(t);}
    Par(string*);
    Par(ParQuo*quo) {init(quo);}
    Par(ParRegx*rx) {init(rx);}
    
    void init();
    void init(MatchType);
    void init(ParQuo*quo);
    void init(ParRegx*rx);
    
    void setMinMax(int,int);
    
    void add(Par*n);

    Par& operator = (Par&p_) ;
    Par& operator [](int index);
  
    void printLevelIndent(int level);
    void printLevelInputMargin(int level, ParDoc&doc);
    
    /* push and pop token stack for backtracking non-matches 
     */
    int  pushTok(Toks*, int level, ParDoc&input);
    void popTok (Toks*, int tokenSizeBefore);

    /* parse a a+ a* a?
     */
    bool parse   (Toks*, ParDoc&, int level);
    
    /* parseAnd may include parseAhead (a ~b c), which in turn calls parseBefore
     */
    void parseBefore(Toks*, ParDoc&, int level, Par *&before, int startIdx, int endIdx);
    bool parseAhead (Toks*, ParDoc&, int level, Par *&before, Par*par);
    bool parseAnd   (Toks*, ParDoc&, int level);
    
    /* parseOr may include ParseAnd with ParseAhead (a | b | c d~ e)
     */
    bool parseOr   (Toks*, ParDoc&, int level);
    
    /* parseWave matches (a ~ b) - does not mix with ParseOr so, 
     * do NOT mix parseOr (a ~ b | c) or Leafs ('a' ~ "b")
     */
    bool parseWave (Toks*, ParDoc&, int level);

    /* One or more Leaf nodes needed to match */
    bool parseQuo (Toks*, ParDoc&, int level);
    bool parseRegx(Toks*, ParDoc&, int level);

    void parseBufToFile (const char*buf, const char*traceFile, bool openStderr);
    void parseFileToFile (const char*inputFile, const char*tracefile);
  };
