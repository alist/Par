/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "ParToks.h"

struct ParParToks:ParToks {

    ParParToks() { ParToks(); initNameHash(0); }
    ParList parParList; // the grammar to parse grammars
    virtual void initNameHash(Par*); // init with Par.boot macros
};