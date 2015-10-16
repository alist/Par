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

struct Par {

    static Toks *_tokens;
    static int _transaction;
    
    int _lastTransaction; //memoize
    
    vector<Par*> _list;
    string _name;
    ParRegx *_parseRegx;
    ParQuo *_parseQuo;

    ParType _type;
    int     _level;
    
    Par(int level_=0){_level=level_;}
    
     // this is phase one    
    
    void add(Par*n);
    void init(ParType type_);    
    void setName(const char*who_);
    
    Par& operator =(Par&p_) ;    
    Par& operator [](int index);    
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
    void printToks();
    bool trace(int level, const char* value=0);   
};
/* kludge to create constructors with up to 16 (Parselex*p) rvalues
*/
#define ParamBlock(x,y)\
x(Par*a) { init(y); add(a); }\
x(Par*a,Par*b) { init(y); add(a); add(b); }\
x(Par*a,Par*b,Par*c) { init(y); add(a); add(b); add(c); }\
x(Par*a,Par*b,Par*c,Par*d) { init(y); add(a); add(b); add(c);add(d); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e) { init(y); add(a); add(b); add(c); add(d); add(e); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k,Par*l) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k,Par*l,Par*m) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(m); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k,Par*l,Par*m,Par*n) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k,Par*l,Par*m,Par*n,Par*o) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); add(o); }\
x(Par*a,Par*b,Par*c,Par*d,Par*e,Par*f,Par*g,Par*h,Par*i,Par*j,Par*k,Par*l,Par*m,Par*n,Par*o,Par*p) { init(y); add(a); add(b); add(c); add(d); add(e); add(f); add(g); add(h); add(i); add(j); add(k); add(l); add(n); add(o); add(p); }\

#define InitRegx(x,y) x(const char*pattern_) { init(kParRegx); _parseRegx = new ParRegx(pattern_);} ParamBlock(x,y)
#define InitQuo(x,y)  x(const char*pattern_) { init(kParQuo);  _parseQuo  = new ParQuo (pattern_);} ParamBlock(x,y)

struct _Mny  : Par { InitRegx(_Mny  ,kParMny ) };
struct _Any  : Par { InitRegx(_Any  ,kParAny ) };
struct _Opt  : Par { InitRegx(_Opt  ,kParOpt ) };
struct _And  : Par { InitRegx(_And  ,kParAnd ) };
struct _Or   : Par { InitRegx(_Or   ,kParOr  ) };
struct _Quo  : Par { InitQuo (_Quo  ,kParQuo ) };
struct _Regx : Par { InitRegx(_Regx ,kParRegx) };

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

#define ParEnum(a,b,c) Tok::enumName[k##c##_##a] = #a; Tok::nameEnum[#a] = k##c##_##a;
#define ParDeclare(a,b) Par *a;
#define ParNew(a,b) a = new Par(); a->setName(#a); 
#define ParBind(a,b) { *a=*b; delete(b); }
