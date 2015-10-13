#import "DefParseDef.h"
#import "DefParseDef.h"
//#import "Tr3Expand.h"
//#import "Tr3Script.h"

#import "DebugPrint.h"
#define PrintParTest(...) DebugPrint(__VA_ARGS__)

void ParTest() {
    
    // create the root Tr3* called SkyRoot.
//    DefParseDef *parse = new DefParseDef();
//    Tr3 *SkyRoot = new Tr3("√");
//    Tr3Expand expand;
//    
//    char *buf = ParDef::parseFileToBuf("test1.tr3");
//
//    if (buf) {
//  
//        parse->parseBuf(buf,/*print*/true);
//        expand.expandTr3Tokens(SkyRoot,parse->tokens);
//        // there may be many 
//        expand.mergeClones(SkyRoot);
//        expand.mergeDuplicates(SkyRoot);
//        expand.connectEdgeDefs(SkyRoot);
//
//        Tr3Script::PrintTr3(stdout, SkyRoot,
//                            /*showBinding*/false,
//                            /*expandEdges*/false,
//                            /*compact*/true);
//    }
}
