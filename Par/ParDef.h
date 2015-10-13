#import "ParQuo.h"
#import "ParRegx.h"
#import "ParDoc.h"
#import "ParNameEnum.h"

#import <vector>
#import <iterator>
#import <string>
#import <stdarg.h>
#import <stdio.h>

typedef enum {
    
    kParUndef = -1,
    kParMny,
    kParAny,
    kParOpt,
    kParAnd,
    kParOr,
    kParQuo,
    kParRegx,
    kParIndent,
    kParRedent,
    kParMax // always last
    
} ParType;

struct ParDef {

    static ParToks *_tokens;
    static int _tokNow;
    static int _transaction;
    
    int _lastTransaction; //memoize
    
    vector<ParDef*> _list;
    string _name;
    ParRegx *_parseRegx;
    ParQuo *_parseQuo;

    ParType _type;
    int     _level;
    
    ParDef(int level_=0){_level=level_;}
    
     // this is phase one    
    
    void add(ParDef*n);
    void init(ParType type_);    
    void setName(const char*who_);
    
    ParDef& operator =(ParDef&p_) ;    
    ParDef& operator [](int index);    
    operator float();    
    operator int();    
    operator const char*();  
    void printLevelIndent(int level);
    void printLevelInputMargin(int level, ParDoc&input);
    int pushParTok(int level, ParDoc&input);
    void pushValue(int level, ParDoc&input);
    void popParTok(int tokenSizeBefore);
    
    int parseCount(ParDoc &input, int level);
    bool parseMny(ParDoc &input, int level);    
    bool parseAny(ParDoc &input, int level);    
    bool parseAnd(ParDoc &input, int level);
    bool parseOr(ParDoc &input, int level);    
    bool parseOpt(ParDoc &input, int level);    
    bool parseQuo(ParDoc &input, int level);    
    bool parseRegx(ParDoc &input, int level);    

    void parseBufToFile (const char*buf, const char*traceFile, bool openStderr);
    void parseFileToFile (const char*inputFile, const char*tracefile);
    static char *parseFileToBuf (const char*inputFile);
    
    bool parse(char*&input);    
    bool parse(ParDoc &input, int level);
    void printToFile (const char*logfile);
    void print(int level);
    void printParToks();
    bool trace(int level, const char* value=0);   
};
/* kludge to create constructors with up to 16 (Parselex*p) rvalues
*/
#define ParamBlock(x,y)\
x(ParDef*a) { init(y); add(a); }\
x(ParDef*a,ParDef*b) { init(y); add(a); add(b); }\
x(ParDef*a,ParDef*b,ParDef*c) { init(y); add(a); add(b); add(c); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d) { init(y); add(a); add(b); add(c);add(d); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e) { init(y); add(a); add(b); add(c); add(d); add(e); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k,ParDef*l) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k,ParDef*l,ParDef*m) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(m); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k,ParDef*l,ParDef*m,ParDef*n) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k,ParDef*l,ParDef*m,ParDef*n,ParDef*o) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); add(o); }\
x(ParDef*a,ParDef*b,ParDef*c,ParDef*d,ParDef*e,ParDef*f,ParDef*g,ParDef*h,ParDef*i,ParDef*j,ParDef*k,ParDef*l,ParDef*m,ParDef*n,ParDef*o,ParDef*p) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); add(o); add(p); }\

#define InitRegx(x,y) x(const char*pattern_) {init(kParRegx); _parseRegx = new ParRegx(pattern_);} ParamBlock(x,y)
#define InitQuo(x,y)  x(const char*pattern_) {init(kParQuo);  _parseQuo  = new ParQuo (pattern_);} ParamBlock(x,y)

struct _Mny  : ParDef { InitRegx(_Mny  ,kParMny ) };
struct _Any  : ParDef { InitRegx(_Any  ,kParAny ) };
struct _Opt  : ParDef { InitRegx(_Opt  ,kParOpt ) };
struct _And  : ParDef { InitRegx(_And  ,kParAnd ) };
struct _Or   : ParDef { InitRegx(_Or   ,kParOr  ) };
struct _Quo  : ParDef { InitQuo (_Quo  ,kParQuo ) };
struct _Regx : ParDef { InitRegx(_Regx ,kParRegx) };

#undef Mny    
#undef Any     
#undef And     
#undef One     
#undef Or      
#undef Opt     
#undef Quo     
#undef Regx    
#undef Indent  
#undef Redent  

#define Mny  new _Mny
#define Any  new _Any
#define And  new _And    
#define One  new _And    
#define Or   new _Or
#define Opt  new _Opt
#define Quo  new _Quo
#define Regx new _Regx

#define ParDeclare(a,b) ParDef *a;
#define ParNew(a,b) a = new ParDef(); a->setName(#a); 
#define ParBind(a,b) { *a=*b; delete(b); }
