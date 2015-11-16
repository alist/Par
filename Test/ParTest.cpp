#import "ParTest.h"
#import "ParDef.h"
#import "ParTok.h"
#import "ParFile.h"

#import "DebugPrint.h"
#define PrintParTest(...) DebugPrint(__VA_ARGS__)

void ParTest::testOne() {
    
    // create the root Tr3* called SkyRoot.
    
    ParDef *parDef = new ParDef();

    parDef->printToks = true;
    parDef->initWithFile("Hello.def");
    //ParFile::redirectStdout2Stderr();
    parDef->traceBuf = true;
    parDef->parseFile("Hello.test");
}

void ParTest::testPar() {
    
    ParDef *parTest = new ParDef();
    parTest->initWithFile("ParTest.def");
    parTest->parseFile("ParTest.test");
    testTokens(parTest);
}

void ParTest::testTokens(ParDef *parTest) {
    
    ParDef *parDef = 0;
    Toks*toks = parTest->parTok->tokens;
    
    typedef enum { kUndef, kHead, kDef, kIn, kOut} Context;
    Context context = kUndef;
    int errors = 0;
    int tests = 0;
    
    for (Tok*tok : *toks) {
        
        switch (tok->tokType) {
                
                // HEAD DEF IN OUT END set the context for the next token "~"
                
            case str2int("HEAD"): context = kHead; break;
            case str2int("DEF"):  context = kDef;  break;
            case str2int("IN"):   context = kIn;   break;
            case str2int("OUT"):  context = kOut;  break;
                
            case str2int("END"):
                
                fprintf(stderr,"\nEND : %i error%s in %i test%s\n\n",
                        errors, (errors > 1 ? "s" : ""),
                        tests,  (tests  > 1 ? "s" : ""));
                return;

            case str2int("~"): {
                
                char *singleLine = ParFile::singleLineFromString(tok->value);
                
                switch (context) {
                        
                    case kUndef: break;
                        
                    case kHead:
                        
                        fprintf(stderr,"\nTEST: %s ",singleLine);
                        break;
                        
                    case kDef:
                        
                        //fprintf(stderr,"DEF : %s\n",singleLine);
                        if (parDef) {
                            delete (parDef);
                        }
                        parDef = new ParDef();
                        parDef->initWithBuf(tok->value->c_str());
                        break;
                        
                    case kIn:
                        
                        //fprintf(stderr,"IN  : %s\n",singleLine);
                        parDef->parseBufTrace(tok->value->c_str(),"results");
                        break;
                        
                    case kOut:
                        
                        //fprintf(stderr,"OUT : %s\n",singleLine);
                        if (!ParFile::compareBufToFile(tok->value->c_str(),"results")) {
                            errors++;
                        }
                        tests++;

                        break;
                        
                }
            }
        }
    }
}

