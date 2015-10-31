#import "Par.h"

/* Create constructors with up to 10 (Par*p) rvalue defined in Par_(...) macros.
 * Still used for bootstrapping the definition of the parser in Def.par file
 * which takes a .def file and creates a vector of Pars that reference each other.
 * These macros are not used for the .def files.
 *
 * TODO: this is the last vestige of the prior version of the Par app, which
 * would always create a .def for from each .par file, which in turn is
 * included multiple times to contruct a static parser class. This meant
 * that a new binary was needed for any modifications to the grammar.
 *
 * With ParPar (not using ParMacro), a modified grammar can be downloaded
 * and deployed without deploying a new binary.
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

#define InitRegx(x,y) x(const char*pattern_) { init(new ParRegx(pattern_));} ParamBlock(x,y)
#define InitQuo(x,y)  x(const char*pattern_) { init(new ParQuo (pattern_));} ParamBlock(x,y)

struct _Mny  : Par { InitRegx(_Mny  ,kRepMny ) };
struct _Any  : Par { InitRegx(_Any  ,kRepAny ) };
struct _Opt  : Par { InitRegx(_Opt  ,kRepOpt ) };
struct _And  : Par { InitRegx(_And  ,kMatchAnd ) };
struct _Or   : Par { InitRegx(_Or   ,kMatchOr  ) };
struct _Quo  : Par { InitQuo (_Quo  ,kMatchQuo ) };
struct _Regx : Par { InitRegx(_Regx ,kMatchRegx) };

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

