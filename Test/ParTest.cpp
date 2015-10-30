#import "ParPar.h"

#import "DebugPrint.h"
#define PrintParTest(...) DebugPrint(__VA_ARGS__)

void ParTest() {
    
    // create the root Tr3* called SkyRoot.
    
    ParPar *parPar = new ParPar();
    parPar->initWithFile("Cal.def");
    parPar->parseFile("Cal.test");
      
}
