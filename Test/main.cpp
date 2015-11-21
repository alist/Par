/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#include <stdio.h>
#include "ParTest.h"

int main(int argc, const char * argv[]) {
    
#if 0
        
        ParTest::helloWorld();
#else
        ParTest::testOne("TestOne");
        ParTest::testSuite("TestSuite");
#endif
    return 0;
}
