/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "TokTypes.h"
#import "Par.h"

struct ParToks {
    
    Toks *toks;
    Par *root;
    
    ParToks()           { toks = new Toks(); }
    ParToks(Par*root_)  { toks = new Toks(); root = root_;  initNameHash(root);}
    ~ParToks();
    
    void printToks(FILE *fp);
    
    virtual void initNameHash(Par*);
    virtual Toks *buf2tok(const char*buf, bool trace, bool print, FILE *fp);
    
};