/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "ParTest.h"
#import "ParPar.h"
#import "ParToks.h"
#import "ParFile.h"

/* test a single defintion, and input and then view the results on the console.
 * <fname>.par:     parser grammar
 * <fname>.test:    test file
 * for exampole
 
 * TestOne.par:
 
    def (hello|world)+ 
    hello('hello') 
    world('world')
 
 * TestOne.test:
 
    hello world
 
 * the console should show:
 
    0, 1: def :
    1, 2:  hello : hello
    2, 1: def :
    3, 2:  world : world

 * to create a new test, use TestOne and copy and paste console results
 * into ParseTest.test, using the formate shown for testSuite, below
 */
 
void ParTest::testOne(const char*fname) {

    string parFile  = fname; parFile  += ".par";
    string testFile = fname; testFile += ".test";
    
    ParPar *parPar = new ParPar();
    parPar->printToks = true;
    parPar->readGrammar(parFile.c_str(),stderr);
    parPar->traceDoc = true;
    parPar->file2Toks(testFile.c_str(),stderr);
}

/* testSuite(fname) tests a series of definitions, inputs, and expected token outputs.
 * <fname>.par:  defines a test suite parser: test (HEAD ~ PAR ~ IN ~ OUT ~ END?)+
 * <fname>.test: provides as series of tests.
 * For example, for ParTest.test:

    [HEAD] Hello World
    [PAR] def (hello | world)+ hello('hello') world('world')
    [IN]  hello world
    [OUT]
    0, 1: def :
    1, 2:  hello : hello
    2, 1: def :
    3, 2:  world : world
    ...
    [END]
 
 * TestSuite is somewhat circular, in that is is using Par to test Par.
 * ParTest.par is a valid Par file 
 
 */
void ParTest::testSuite(const char*fname) {
    
    string parFile  = fname; parFile  += ".par";
    string testFile = fname; testFile += ".test";

    ParPar *parTest = new ParPar();
    parTest->readGrammar(parFile.c_str(),stderr);
    parTest->buf2Toks(testFile.c_str(),stdout);
    testTokens(parTest);
}

/* Called by testSuite to test a series of alternating
 * [HEAD] ... [PAR] ... [IN] ... [OUT] ... END, see above
 *
 * For example, for ParTest.test, show above, 
 * the output to the console:
 
    TEST: Hello World  ✔︎ ✔︎ ✔︎
    TEST: case sensitive  ✔︎
    TEST: surrounding parethesis  ✔︎ ✔︎
    TEST: Mixed Emoji  ✔︎ ✔︎
    TEST: wave (a ~ c ~ e?)+  ✔︎ ✔︎ ✔︎ ✔︎ ✔︎
    TEST: range (a b{2,3} c)  ✔︎
    TEST: lookahead (a ~bc de)  ✔︎ ✔︎
    TEST: lookahead (a ~b c ~d e)+  ✔︎ ✔︎ ✔︎ ✔︎ ✔︎
    END : 0 error in 21 tests
 *
*/
void ParTest::testTokens(ParPar *parTest) {
    
    ParPar *parPar = 0;
    Toks*toks = parTest->parToks->toks;
    
    typedef enum { kUndef, kHead, kPar, kIn, kOut} Context;
    Context context = kUndef;
    int errors = 0;
    int tests = 0;
    
    for (Tok*tok : *toks) {
        
        switch (tok->tokType) {
                
                // HEAD PAR IN OUT END set the context for the next token "~"
                
            case str2int("HEAD"): context = kHead; break;
            case str2int("PAR"):  context = kPar;  break;
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
                        
                    case kPar:
                        
                        //fprintf(stderr,"PAR : %s\n",singleLine);
                        if (parPar) {
                            delete (parPar);
                        }
                        parPar = new ParPar();
                        parPar->buf2Grammar(tok->value->c_str(),stdout);
                        break;
                        
                    case kIn:
                        
                        //fprintf(stderr,"IN  : %s\n",singleLine);
                        parPar->parseBuf2File(tok->value->c_str(),"results");
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

