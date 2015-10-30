#import "ParTokDef.h"
#import "Tok.h"

void ParTokDef::initToks(Par*par) {

    int64_t num;

#define Par_(a,b)  num = str2int(#a); Tok::enumName[num] = #a; Tok::nameEnum[#a] = num;
#include "Def.par"
#undef Par_
   
#define Par_(a,b) ParNew(a,b)  // a = new Par(); a->setName(#a);
#include "Def.par" 
#undef Par_
    
#define Par_(a,b) ParBind(a,b)  //  { *a=*b; delete(b); }
#include "Def.par"
#undef Par_
    
    root = def;
}

