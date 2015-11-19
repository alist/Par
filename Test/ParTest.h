/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "ParPar.h"

struct ParTest {
    
    static void testOne(const char*fname);
    static void testSuite(const char*fname); // series of test via [TEST] [IN] [OUT]
    static void testTokens(ParPar *parPar);  // 
};