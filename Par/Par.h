/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

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

/* A ParList can be either be
 * 1) list of pars for a single statement, a has a parList" {b, c} in "a (b c)"
 * 2) or a complete set of pars to make up a grammar {a,b,c}
 */
typedef vector<Par*> ParList;

class Par {
    
public:
    
    string name;
    MatchType matching;
    ParList parList; // {b,c} in "a (b c)"
    
    int maxRepeat;   // 1: a  a?  -1: * + (up to Par::MaxRepeatDefault)
    int minRepeat;   // 0: a* a?   1: a a+
    
    union {
        ParRegx *regx;
        ParQuo  *quo;
    } match;
    
    int memoMe; //memoize
    static int MemoNow;
    static int MaxRepeatDefault; // max matches for a* or a+
    static int MaxLevelDefault; // max levels deep 
    static bool Trace; // trace the parse as it happens

    /* Used only by ParBoot Par_() to bootstrap the parser
     */
    Par(){init();};
    void setName(const char*who_);
    void init(RepeatType);
    
    /* Used while parsing *.par files 
     */
    Par(MatchType t){init(t);}
    Par(string*n)   {init(); name = *n; }
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
  
     /* parse a a+ a* a?
     */
    bool parse(Toks&, ParDoc&, int level, Par *&before);
    
private:
    
    void printLevelIndent(int level);
    void printLevelInputMargin(int level, ParDoc&doc);
    
    /* push and pop token stack for backtracking non-matches 
     */
    int  pushTok(Toks&, int level, ParDoc&input);
    void popTok (Toks&, int tokenSizeBefore);

    
    /* parseAnd may include parseAhead (a ~b c), which in turn calls parseBefore
     */
    void parseBefore(Toks&, ParDoc&, int level, Par *&before, int startIdx, int endIdx);
    bool parseAhead (Toks&, ParDoc&, int level, Par *&before, Par*par);
    bool parseAnd   (Toks&, ParDoc&, int level, Par *&before);
    
    /* parseOr may include ParseAnd with ParseAhead (a | b | c d~ e)
     */
    bool parseOr   (Toks&, ParDoc&, int level, Par *&before);
    
    /* parseWave matches (a ~ b) - does not mix with ParseOr so, 
     * do NOT mix parseOr (a ~ b | c) or Leafs ('a' ~ "b")
     */
    bool parseWave (Toks&, ParDoc&, int level, Par *&before);

    /* One or more Leaf nodes needed to match */
    bool parseQuo (Toks&, ParDoc&, int level);
    bool parseRegx(Toks&, ParDoc&, int level);
  };
