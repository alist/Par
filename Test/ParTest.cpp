/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import "ParTest.h"
#import "ParPar.h"
#import "ParFile.h"

 /* By popular demand ... hello world
  * starting with two files HelloWold.par and .txt
 
  * HelloWorld.par:
 
    def (hello|world)+ 
    hello('^hello')
    world('^world')
 
  * HelloWorld.txt:
 
    hello world
 
  * the console should show:
 
    0, 1: def :
    1, 2:  hello : hello
    2, 1: def :
    3, 2:  world : world
*/

void ParTest::helloWorld() {
 
    ParPar *parPar = new ParPar();

    parPar->parFile2Grammar("HelloWorld.par",stderr);
    parPar->txtFile2Tokens ("HelloWorld.txt",stderr,/*trace*/true);
    Tok::printToks(parPar->toks, stderr);
}

/* Test a single defintion, and input and then view the results on the console.
 * <fname>.par:    parser grammar
 * <fname>.txt:    text file
 * To create a new test, use TestOne and copy and paste console results
 * into ParseTest.txt, using the formate shown for testSuite, below
 *
 * If there are any problems with the tokenizing, can trace what 
 * the doc is point at by setting: txtFile2Tokens(...,...,true)
 * resulting in the an output that looks like this:

    ⦙a b c d  ⦙   1: def
    ⦙b c d    ⦙   2:  a: a
    ⦙d        ⦙   2:  c: c
 
 * To show the resulting token invoke: Tok::printToks(...)
 * which whill print out a token stream id,level: name: value
 
    0, 1: def :
    1, 2:  a : a
    2, 2:  ~ : b
    3, 2:  c : c
    4, 2:  ~ : d
 */
 
void ParTest::testOne(const char*fname) {

    string parFile  = fname; parFile  += ".par";
    string testFile = fname; testFile += ".txt";
    
    ParPar *parPar = new ParPar();
    parPar->parFile2Grammar(parFile.c_str(),stderr);
    Tok::printToks(parPar->toks, stderr);

    parPar->txtFile2Tokens(testFile.c_str(),stderr,/*trace*/true);
    Tok::printToks(parPar->toks, stderr);
}

/* testSuite(fname) tests a series of grammars, inputs, and token outputs.
 * <fname>.par:  defines the grammar: test (HEAD ~ PAR ~ IN ~ OUT ~ END?)+
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
 
 * testSuite is somewhat circular, in that is is using Par to test Par.
 * ParTest.par is a valid Par file
 */
void ParTest::testSuite(const char*fname) {
    
    string parFile  = fname; parFile  += ".par";
    string testFile = fname; testFile += ".test";

    ParPar *parSuite = new ParPar();
    parSuite->parFile2Grammar(parFile.c_str(),stderr);
    parSuite->txtFile2Tokens(testFile.c_str(),stderr);
    testTokens(parSuite);
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
void ParTest::testTokens(ParPar *parSuite) {
    
    ParPar *parTest = 0;
    
    typedef enum { kUndef, kHead, kPar, kIn, kOut} Context;
    Context context = kUndef;
    int errors = 0;
    int tests = 0;
    
    for (Tok*tok : parSuite->toks) {
        
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
                        
                    case kPar: {
                        
                         if (parTest) {
                            delete (parTest);
                        }
                        parTest = new ParPar();
                        const char *parBuf = tok->value->c_str();
                        parTest->parBuf2Grammar(parBuf,stdout);
                        break;
                    }
                    case kIn: {
                        
                        const char *inputBuf = tok->value->c_str();
                         parTest->txtBuf2TokFile(inputBuf,"results");
                        break;
                    }
                    case kOut: {
                        
                        const char *expectedBuf = tok->value->c_str();
                        if (!ParFile::compareBufToFile(expectedBuf,"results")) {
                            errors++;
                        }
                        tests++;

                        break;
                    }
                }
            }
        }
    }
}

