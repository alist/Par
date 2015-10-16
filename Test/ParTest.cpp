#import "ParTokDef.h"
#import "ParTokDef.h"

#import "DebugPrint.h"
#define PrintParTest(...) DebugPrint(__VA_ARGS__)

void ParTest() {
    
    // create the root Tr3* called SkyRoot.
    ParTokDef *parse = new ParTokDef();
    
    char *buf = Par::parseFileToBuf("Hello.def");

    if (buf) {
        parse->parseBuf(buf,/*print*/true);
    }
}
