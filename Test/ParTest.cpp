#import "ParDef.h"

#import "DebugPrint.h"
#define PrintParTest(...) DebugPrint(__VA_ARGS__)

void ParTest() {
    
    // create the root Tr3* called SkyRoot.
    
    ParDef *parDef = new ParDef();
    parDef->initWithFile("Cal.def");
    parDef->parseFile("Cal.test");
      
}
