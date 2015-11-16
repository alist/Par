#import <stdio.h>
#import <string>

struct ParFile {
    
    static int fd;
    static fpos_t pos;
    static void redirectStdout2Stderr();
    static void redirectStdout(const char *file);
    static void revertStdout();
    static char *readInputFile(const char*inputFile);
    static bool compareBufToFile(const char *buf, const char *file);
    static char *singleLineFromString(std::string*str);
    static void printError(char*buf1, char*buf2, int i1, int i2, int row,
                           bool showFullResult = false);
};