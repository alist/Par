#include <stdio.h>
#include "ParTest.h"

int main(int argc, const char * argv[]) {
#if 0
    ParTest::testOne();
#else
    ParTest::testPar();
#endif
    return 0;
}
