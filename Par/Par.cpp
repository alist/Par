#import "Par.h"
#import "Tok.h"
#import <stdarg.h>
#import <stdio.h>

#import "DebugPrint.h"

#define PrintParseTok(...) Debug(if (Par::Trace) { DebugPrint(__VA_ARGS__); })

int Par::MemoNow=0;
bool Par::Trace=false;

void Par::add(Par*n) {
    
    pars.push_back(n);
}
void Par::init() {

    name = "?";
    repeat = kRepOne;
    matching = kMatchAnd;
    match.regx = 0;
    memoMe = -1;
}
void Par::init(RepeatType repeat_) {
    
    init();
    repeat = repeat_;
}
void Par::init(MatchType matching_) {
    
    init();
    matching = matching_;
}
void Par::init(ParRegx*rx) {
    init();
    matching = kMatchRegx;
    match.regx = rx;
}
void Par::init(ParQuo*quo) {
    init();
    matching = kMatchQuo;
    match.quo = quo;
}

Par::Par(string*name_) {
    init();
    name = *name_;
}

Par::Par(Par*par_) {
    
    init();
    name     = par_->name;
    repeat   = par_->repeat;
    matching = par_->matching;
    pars     = par_->pars;
}


void Par::setName(const char*name_) {
    name = name_;
}

Par& Par::operator =(Par&p_)  {
    
    //name       = p_.name; // do not copy name
    repeat     = p_.repeat;
    pars       = p_.pars;
    matching   = p_.matching;
    match.regx = p_.match.regx;
    match.quo  = p_.match.quo;
    return *this;
}
Par &Par::operator [](int index) {
    
    return *pars[index];
}
Par::operator float() {
    
    return (match.regx ? (float)(*match.regx) : 
            match.quo  ? (float)(*match.quo)  :  -1);
}
Par::operator int() {
    
    return (match.regx ? (int)(*match.regx) : 
            match.quo  ? (int)(*match.quo)  :  -1);
}
Par::operator const char *() {
    
    return (match.regx ? (const char*)(*match.regx) : 
            match.quo  ? (const char*)(*match.quo)  : 0);
}

inline void Par::printLevelIndent(int level) {
    
    for (int i =0; i<level; i++) {
        PrintParseTok(" ");
    }
}
inline void Par::printLevelInputMargin(int level, ParDoc&input) {

    // print a slice of the input to parse
    PrintParseTok("⦙");
    char *cp =input._char;
    for (int i=0; i<9; i++) {
        
        if (*cp=='\n' || *cp=='\r') {
            PrintParseTok("⤶");
            cp++;
        }
        else if (*cp!='\0') {
            PrintParseTok("%c",(char)*cp);
            cp++;
        }
        else {
            PrintParseTok(" ");
        }
    }
    // print the level number and indent
    PrintParseTok("⦙ %3i:",level);
    printLevelIndent(level);
}

int Par::pushTok(Toks*toks, int level, ParDoc&input) {
    
    int ret = (int)toks->size();
    if (name.size()>0 && name[0]!='?') {
        
        Tok*token = new  Tok(name,0,level);
        toks->push_back(token);
        
        printLevelInputMargin(level,input);
        PrintParseTok("%s\n",name.c_str());
    }
    return ret;
}

#define PushTok ParDoc prev = input; int tokenSizeBefore = pushTok(toks,level,input);
#define PopTok  input = prev; popTok(toks,tokenSizeBefore);

void Par::popTok(Toks*toks, int tokenSizeBefore) {
    
    while (toks->size()>tokenSizeBefore) {
        
        Tok*token = toks->back();
        delete (token);
        toks->pop_back();
    }
}


bool Par::parse(Toks*toks, ParDoc &input, int level) {
    
    switch (repeat) { //TODO: One(item) is workaround to line up levels

        default:
        case kRepOne: return parseOne(toks,input,level+1);
        case kRepMny: return parseMny(toks,input,level+1);
        case kRepAny: return parseAny(toks,input,level+1);
        case kRepOpt: return parseOpt(toks,input,level+1);
    }
}

bool Par::parseOne(Toks*toks, ParDoc &input, int level) {
    
    switch (matching) {
            
        case kMatchRegx:return parseRegx(toks,input,level);
        case kMatchQuo: return parseQuo (toks,input,level);
        case kMatchAnd: return parseAnd (toks,input,level);
        case kMatchOr:  return parseOr  (toks,input,level);
    }
}

bool Par::parseMny(Toks*toks, ParDoc &input, int level) {
    
    PushTok
    
    int count = 0;
    for (bool parsing=true; parsing;) {
        
        switch (matching) {
                
            case kMatchRegx: parsing = parseRegx(toks,input,level); break;
            case kMatchQuo:  parsing = parseQuo (toks,input,level); break;
            case kMatchAnd:  parsing = parseAnd (toks,input,level); break;
            case kMatchOr:   parsing = parseOr  (toks,input,level); break;
        }
        if (parsing) {
            count++;
        }
    }
    if (count>0) {
        return true;
    }
    else {
        PopTok
        return false;
    }
}
bool Par::parseAny(Toks*toks, ParDoc &input, int level) {

    // parse pars
    
    for (bool parsing=true; parsing;) {
        
        switch (matching) {
            
            case kMatchRegx: parsing = parseRegx(toks,input,level); break;
            case kMatchQuo:  parsing = parseQuo (toks,input,level); break;
            case kMatchAnd:  parsing = parseAnd (toks,input,level); break;
            case kMatchOr:   parsing = parseOr  (toks,input,level); break;
        }
    }
    return true;
}

bool Par::parseOpt(Toks*toks, ParDoc &input, int level) {
    
    switch (matching) {
        
        case kMatchRegx: parseRegx(toks,input,level); break;
        case kMatchQuo:  parseQuo (toks,input,level); break;
        case kMatchAnd:  parseAnd (toks,input,level); break;
        case kMatchOr:   parseOr  (toks,input,level); break;
    }
    return true;
}

#pragma mark - and or

bool Par::parseAnd(Toks*toks, ParDoc &input, int level) {

    if (pars.size()==0) {
        return false;
    }
    PushTok
    for (Par *par : pars) {
       
        if (!par->parse(toks, input,level)) {
            
            PopTok
            return false;
        }        
    }
    return true;
}

bool Par::parseOr(Toks*toks, ParDoc &input, int level) {
    
    if (pars.size()==0) {
        return false;
    }
    PushTok
    for (Par *par : pars) {
        
        if (par->parse(toks, input,level)) {
            return true;
        }
     }
    PopTok

    return false;
}        

#pragma mark - parse leaf

bool Par::parseQuo(Toks*toks, ParDoc &input, int level) {
    
    if (match.quo && match.quo->parse(input)) {
        
        pushTok(toks,level,input);
        return true;
        
    } else {
        
        return false;
    }
} 
bool Par::parseRegx(Toks*toks, ParDoc &input, int level) {
    
    if (match.regx && match.regx->parse(input)) {
        
        // ignore discardible regex such as r'^[\\(]' in (r'^[\\(]' (who why*)+ r'^[\\)]')
        
        if (name!="?") {
            
            printLevelInputMargin(level, input);
            PrintParseTok("%s: %s\n", name.c_str(),(const char*)*match.regx);
            
            Tok*token = new Tok(name,(const char*)*match.regx,level);
            toks->push_back(token);
        }
        return true;
        
    } else  {
        
        return false;
    }
}
char * Par::readFile(const char*inputFile) {
    
    MemoNow++;
    
    FILE *file = freopen(inputFile, "r", stderr);
    
    if (file) {
        
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buf = (char*)malloc(fileSize+1);
        fread((void*)buf, fileSize, 1, file);
        buf[fileSize]='\0';
        return buf;
    }
    else {
        PrintParseTok("\n *** file:%s not found",inputFile);
        return 0;
    }
}


