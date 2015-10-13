#import "ParDef.h"
#import "ParTok.h"
#import <stdarg.h>
#import <stdio.h>

#define Trace(...)
#define PrintParseTok(...) //DebugPrint(__VA_ARGS__)

ParToks * ParDef::_tokens=0;
int ParDef::_tokNow=0;
int ParDef::_transaction=0;
const char*Unknown = "?";

void ParDef::add(ParDef*n) {
    
    n->_level = _level+1;
    _list.push_back(n); 
}
void ParDef::init(ParType type_) {
    
    _name = Unknown;
    _parseRegx = 0;
    _parseQuo = 0;
    _type = type_;
    _lastTransaction = _transaction;
    ParTok::initMapOnce();
}
void ParDef::setName(const char*name_) {
    _name = name_;
}
ParDef& ParDef::operator =(ParDef&p_)  {
    
    //_name       = p_._name; // do not copy name
    _list       = p_._list;
    _parseRegx  = p_._parseRegx;
    _parseQuo   = p_._parseQuo;
    _level      = p_._level;
    _type       = p_._type;
    return *this;
}
ParDef &ParDef::operator [](int index) {
    
    return *_list[index];
}
ParDef::operator float() {
    
    return (_parseRegx ? (float)(*_parseRegx) : 
            _parseQuo  ? (float)(*_parseQuo)  :  -1);
}
ParDef::operator int() {
    
    return (_parseRegx ? (int)(*_parseRegx) : 
            _parseQuo  ? (int)(*_parseQuo)  :  -1);
}
ParDef::operator const char *() {
    
    return (_parseRegx ? (const char*)(*_parseRegx) : 
            _parseQuo  ? (const char*)(*_parseQuo)  : 0);
}

inline void ParDef::printLevelIndent(int level) {
    
    for (int i =0; i<level; i++) {
        PrintParseTok(" ");
    }
}
inline void ParDef::printLevelInputMargin(int level, ParDoc&input) {

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

int ParDef::pushParTok(int level, ParDoc&input) {
    
    int ret = (int)_tokens->size();
    if (_name.size()>0 && _name[0]!='?') {
        
        ParTok*token = new  ParTok(_name,0,level);
        _tokens->push_back(token);
        
        printLevelInputMargin(level,input);
        PrintParseTok("%s\n",_name.c_str());
    }
    return ret;
}

void ParDef::pushValue(int level, ParDoc&input) {

    // ignore discardible regex such as r'^[\\(]' in (r'^[\\(]' (who why*)+ r'^[\\)]')
    if (_name=="?") {
        return;
    }
    
    printLevelInputMargin(level, input);
    PrintParseTok("%s: %s\n", _name.c_str(),(const char*)*_parseRegx);
    
    ParTok*token = new ParTok(_name,(const char*)*_parseRegx,level);
    _tokens->push_back(token);
}

#define PushParTok ParDoc prev = input; int tokenSizeBefore = pushParTok(level,input);
#define PopParTok  input = prev; popParTok(tokenSizeBefore);

void ParDef::popParTok(int tokenSizeBefore) {
    
    while (_tokens->size()>tokenSizeBefore) {
        
        ParTok*token = _tokens->back();
        delete (token);
        _tokens->pop_back();
    }
}
inline int ParDef::parseCount(ParDoc &input, int level) {

    int count = 0;
    for (bool parsing=true; parsing;) {
        
        for (vector<ParDef*>::iterator it  = _list.begin(); it != _list.end(); it++) {
            
            if ((*it)->parse(input,level)) {
                
                count ++;
                
            } else {
                
                parsing = false;
                break;
            }
        }
    }
    return count;
}

bool ParDef::parseMny(ParDoc &input, int level) {
    
    Trace(level)
    
    PushParTok
    if (parseCount(input, level)>0) {
        return true;
    }
    else {
        PopParTok
        return false;
    }
}
bool ParDef::parseAny(ParDoc &input, int level) {
    
    PushParTok
    if (parseCount(input, level)>0) {
        return true;
    }
    else {
        PopParTok
        return true;
    }
}
bool ParDef::parseAnd(ParDoc &input, int level) {
    
    Trace(level)
    
    PushParTok  
    for (vector<ParDef*>::iterator it = _list.begin(); it != _list.end();  it ++) {
        
        if (!(*it)->parse(input,level)) {
            
            PopParTok 
            return false;
        }        
    }
    return true;
}           
bool ParDef::parseOr(ParDoc &input, int level) {
    
    Trace(level)
    
    for (vector<ParDef*>::iterator it = _list.begin(); it != _list.end(); it++) {
        
        PushParTok 
        if ((*it)->parse(input,level)) {
            return true;
        }
        PopParTok
    }
    return false;
}        
bool ParDef::parseOpt(ParDoc &input, int level) {
    
    Trace(level)
    
    for (vector<ParDef*>::iterator it  = _list.begin(); it != _list.end(); it++)   {
        
        PushParTok          
        if ((*it)->parse(input,level)) {
            return true;
        }
        PopParTok
    }
    return true;
} 
bool ParDef::parseQuo(ParDoc &input, int level)  {
    
    if (_parseQuo && _parseQuo->parse(input)) {
        
        Trace(level,(const char*)*_parseQuo._pattern)
        pushParTok(level,input);
        return true;
        
    } else {
        
        return false;
    }
} 
bool ParDef::parseRegx(ParDoc &input, int level) {
    
    if (_parseRegx && _parseRegx->parse(input)) {
        
        Trace(level,(const char*)*_parseRegx)   
        pushValue(level,input);
        return true;
        
    } else  {
        
        return false;
    }
} 

void ParDef::parseBufToFile (const char*buf, const char*traceFile, bool openStderr) {
    
    if (openStderr)
        freopen(traceFile, "w", stderr);    
    ParDoc input((char*)buf);
    //bool completed = parse(input,0);
    input.eatWhitespace();
    
    if (true)/*(!completed || *input._char != '\0')*/ {
        
        PrintParseTok("\nstopped starting at row: %-3i col: %-3i string: ",input._row, input._col);
        // print string where failed occurred, until end of line
        for (char*s=input._char; *s!='\0' && *s!='\n'; s++) {
            PrintParseTok("%c",*s);
        }
        PrintParseTok("\n          ending at row: %-3i col: %-3i string: ",input._deepest._row, input._deepest._col);
        
        // print string where failed occurred, until end of line
        for (char*s=input._deepest._char; *s!='\0' && *s!='\n'; s++) 
            PrintParseTok("%c",*s);
        PrintParseTok("\n");
        
    } else {
        
        PrintParseTok("\ndone.\n");
    }
    printParToks();
}

void ParDef::parseFileToFile (const char*inputFile, const char*tracefile) {
    
    char *buf = parseFileToBuf(inputFile);
    parseBufToFile(buf,tracefile,/*openStderr*/true);
    delete buf;
}

char * ParDef::parseFileToBuf (const char*inputFile) {
    
    _transaction++;
    
    static FILE *file = freopen(inputFile, "r", stderr);
    
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

bool ParDef::parse(char*&buf) {
    
    ParDoc input(buf);
    return parse(input,0);
}
bool ParDef::parse(ParDoc &input, int level) {
    
    bool ret; 
    
    switch (_type) { //TODO: One(item) is workaround to line up levels
            
        case kParRegx: ret = parseRegx(input,level+1);  break;
        case kParQuo : ret = parseQuo (input,level+1);  break;
        case kParMny : ret = parseMny (input,level+1);  break;
        case kParAny : ret = parseAny (input,level+1);  break;
        case kParAnd : ret = parseAnd (input,level+1);  break;
        case kParOr  : ret = parseOr  (input,level+1);  break;
        case kParOpt : ret = parseOpt (input,level+1);  break;
        default      : ret = false;                     break;
    }
    if (ret == true)
        return true;
    return ret;
}
void ParDef::printToFile (const char*logfile) {
    
    _transaction++;
    
    freopen(logfile, "w", stderr);
    for (vector<ParDef*>::iterator it = _list.begin(); it != _list.end(); it++) {
        
        (*it)->print(1);
    }
}
void ParDef::print (int level) {
    
    trace(level);     
    
    if (_lastTransaction!=_transaction) {
        _lastTransaction= _transaction;
    }
    else {
        PrintParseTok("*** loop found in definition so stopping here *** ");
        return;
    }
    for (vector<ParDef*>::iterator it =_list.begin(); it != _list.end(); it++) {
        
        (*it)->print(level+1);
    }
}

void ParDef::printParToks() {
    
    // show the results of a parse, as list of tokens
    
    PrintParseTok("\n-------------------dump tokens-------------------\n");

    for (ParTok*tok : *_tokens) {
        
        PrintParseTok("%4i", tok->tokId);
        printLevelIndent(tok->level);
        
        //const char* nodeName = TokNames[tok->tokType].c_str();
        if (tok->value->size()>0)
            PrintParseTok("%s: %s\n",nodeName,tok->value->c_str());
        else 
            PrintParseTok("%s\n",nodeName);
    }
}
bool ParDef::trace(int level,const char *value) {
    
    // show lex parsing process, in progress, that yields tokens
    
    const char * type;
    switch (_type) {
            
        case kParRegx    : type = "Regx   ";  break;
        case kParQuo     : type = "Quo    ";  break;
        case kParMny     : type = "Mny    ";  break;
        case kParAny     : type = "Any    ";  break;
        case kParAnd     : type = "And    ";  break;
        case kParOr      : type = "Or     ";  break;
        case kParOpt     : type = "Opt    ";  break;
        case kParIndent  : type = "Indent ";  break;
        case kParRedent  : type = "Redent ";  break;
        default         : type = "";         break;
    }
    const char * name = _name.c_str();
    
    if (value && name[0]!='?')  {
        
        printLevelIndent(level);
        PrintParseTok("%s: %s\n",name,value);
        
    } else if (name[0]!='?') {
        
        printLevelIndent(level);
        PrintParseTok("%s\n",name);
        
    } else {
        //printLevelIndent(level);
        //PrintParseTok("%s\n",type);   
    }
    return true;
}
