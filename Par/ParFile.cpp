#import "ParFile.h"
#import "ParTokDef.h"

#import "DebugPrint.h"
#define PrintParFile(...) DebugPrint(__VA_ARGS__)

int ParFile::fd = 0;
fpos_t ParFile::pos = 0;


/* redirect stdout to stderr */
void ParFile:: redirectStdout2Stderr() {
    
    dup2(STDERR_FILENO, STDOUT_FILENO);  //redirects stderr to stdout below this line.
}
void ParFile::redirectStdout(const char *file) {
    
    fflush(stdout);
    fgetpos(stdout, &pos);
    fd = dup(fileno(stdout));
    freopen(file, "w", stdout);
}

void ParFile::revertStdout() {
    
    fflush(stdout);
    dup2(fd, fileno(stdout));
    close(fd);
    clearerr(stdout);
    fsetpos(stdout, &pos);
}

char *ParFile::readInputFile(const char*inputFile) {
    
    FILE *file = freopen(inputFile, "r", stdout);
    
    if (file) {
        
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buf = (char*)malloc(fileSize+2);
        fread((void*)buf, fileSize, 1, file);
        buf[fileSize] = '\0';
        buf[fileSize+1] = '\0'; // kludge
        return buf;
    }
    else {
        PrintParFile("\n *** file:%s not found\n",inputFile);
        return 0;
    }
}

inline bool isWhitespace(char c) {
    
    switch (c) {
            
        case '\t':
        case '\n':
        case '\r':
        case  ' ':
            
            return true;
            
        default:
        case '\0':
            
            return false;;
    }
}

inline bool isWhitespaceEof(char c) {
    
    switch (c) {
            
        case '\t':
        case '\n':
        case '\r':
        case  ' ':
        case '\0':
            
            return true;
            
        default:
            
            return false;
    }
}

char *trimWhitespace(char*buf) {
    
    if (!buf) {
        return 0;
    }
    for (;*buf && isWhitespace(*buf); buf++) {
        continue;
    }
    size_t len = strlen(buf);
    if (len<1) {
        return 0;
    }
    char *normBuf= (char*)malloc(len+1);
    strcpy(normBuf, buf);
    for (;len>1 && isWhitespace(normBuf[len-1]); len--) {
        normBuf[len-1]='\0';
    }
    return normBuf;
 }
char *ParFile::singleLineFromString(string*str) {
    
    const char *v = str->c_str();
    static char buf[180];
    strncpy(buf, v, sizeof(buf)-1);
    for (int i=0; i<sizeof(buf)-1; i++) {
        if (buf[i]==0) {
            break;
        }
        if (buf[i]<32 || buf[i]>127) {
            buf[i] = ' ';
        }
    }
    buf[sizeof(buf)-1]='\0';
    return buf;
}


void ParFile::printError(char*buf1, char*buf2, int i1, int i2, int row,
                         bool showFullResult) {
    
    fprintf(stderr,"\n🚫 mismatch at row:%i\n", row);
    
    // index j -- move backwards to end of prev line ----- move forwards past whitespace of this line
    int j1; for (j1=i1-1; j1>0 && buf1[j1]!='\n'; j1--) {} while(buf1[j1]==' '|| buf1[j1]=='\n') {j1++;}
    int j2; for (j2=i2-1; j2>0 && buf2[j2]!='\n'; j2--) {} while(buf2[j2]==' '|| buf2[j2]=='\n') {j2++;}
    
    // index --- move forwards until end of this line
    int k1; for (k1=i1; buf1[k1] && buf1[k1]!='\n'; k1++) {}
    int k2; for (k2=i2; buf2[k2] && buf2[k2]!='\n'; k2++) {}
    
    // temporarily terminate the end both lines
    char pushBuf1=buf1[k1]; buf1[k1]='\0';
    char pushBuf2=buf2[k2]; buf2[k2]='\0';
    
    // print both buffers differ with an arrow pointing to where they begin to differ
    fprintf(stderr,"%s\n",buf1+j1);
    if (!showFullResult) {
        fprintf(stderr,"%s\n",buf2+j2);
    }
    fprintf(stderr,"%*s▲\n",i1-j1-1," ");
    
    // reinstate from the temporary line terminators
    buf1[k1] = pushBuf1;
    buf2[k2] = pushBuf2;

    if (showFullResult) {
        fprintf(stderr,"%s\n",buf2);
    }
}

bool ParFile::compareBufToFile(const char *buf, const char *file) {

    char *buf1 = trimWhitespace((char*)buf);
    if (!buf1) {
        fprintf(stderr, " compareBufToFile(err,): buf is empty\n");
        return false;
    }
    char *buf2=0;
    char *buf22 = readInputFile(file);
    if (buf22) {
        buf2 = trimWhitespace(buf22);
        free(buf22);
    }
    if (!buf2) {
        free(buf1);
        fprintf(stderr, " compareBufToFile(,err): file is empty\n");
        return false;
    }
    
    int i1=0, i2=0; // indexes for buf1 & buf2
    int row2=0;     // row for buf2
    
    // alternate whitespace and chars so that
    // "a bc d" doesn't match
    // "ab cd"
    for (; buf1[i1] && buf2[i2];) {
        
        for (;buf1[i1] == buf2[i2]; i1++,i2++) {
            if (isWhitespaceEof(buf1[i1]) ||
                isWhitespaceEof(buf2[i2]))
                break;
        }
        for (;isWhitespace(buf1[i1]);i1++) {}
        for (;isWhitespace(buf2[i2]);i2++) {if (buf2[i2]=='\n') row2++;}
       
        if (buf[i1]=='\0' && buf[i2]=='\0') {
            goto done;
        }
        if (buf1[i1] != buf2[i2]) {
            
            printError(buf1, buf2, i1, i2, row2, true);
            free(buf1);
            free(buf2);
            return false;
        }
    }
done:

    free(buf1);
    free(buf2);
    fprintf(stderr,"✔︎ ");
    return true;
}





