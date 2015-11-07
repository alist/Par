#import "Par.h"
#import "Tok.h"
#import <stdarg.h>
#import <stdio.h>

#import "DebugPrint.h"

#define PrintParseTok(...) Debug(if (Par::Trace) { DebugPrint(__VA_ARGS__); })

int  Par::MemoNow=0;
bool Par::Trace=false;
int Par::MaxCountDefault = 99; // max matches for a* or a+
int Par::MaxLevelDefault =200; // max levels deep into parse tree

void Par::add(Par*n) {
    
    parList.push_back(n);
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
    parList  = par_->parList;
}

void Par::setName(const char*name_) {
    name = name_;
}

Par& Par::operator =(Par&p_)  {
    
    //name       = p_.name; // do not copy name
    repeat     = p_.repeat;
    parList       = p_.parList;
    matching   = p_.matching;
    match.regx = p_.match.regx;
    match.quo  = p_.match.quo;
    return *this;
}

Par &Par::operator [](int index) {
    
    return *parList[index];
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

inline void Par::printLevelInputMargin(int level, ParDoc&doc) {

    // print a slice of the doc to parse
    PrintParseTok("⦙");
    char *cp =doc.ptr();
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

#pragma mark - push pop tok 

#define PushTok ParDoc prev = doc; int tokenSizeBefore = pushTok(toks,level,doc);
#define PopTok  doc = prev; popTok(toks,tokenSizeBefore);

inline int Par::pushTok(Toks*toks, int level, ParDoc&doc) {
    
    int ret = (int)toks->size();
    if (name.size()>0 && name[0]!='?') {
        
        Tok*token = new  Tok(name,0,level);
        toks->push_back(token);
        
        printLevelInputMargin(level,doc);
        PrintParseTok("%s\n",name.c_str());
    }
    return ret;
}

inline void Par::popTok(Toks*toks, int tokenSizeBefore) {
    
    while (toks->size()>tokenSizeBefore) {
        
        Tok*token = toks->back();
        delete (token);
        toks->pop_back();
    }
}


RetFlag Par::parse(Toks*toks, ParDoc &doc, int level) {
    
    if (level>MaxLevelDefault) {
        PrintParseTok("\n*** %s:parse exceeded maximum levels:%i***\n",name.c_str(),MaxLevelDefault);
        exit(-1);
    }
    switch (repeat) {

        default:
        case kRepOne: return parseOne(toks,doc,level+1);
        case kRepMny: return parseMny(toks,doc,level+1);
        case kRepAny: return parseAny(toks,doc,level+1);
        case kRepOpt: return parseOpt(toks,doc,level+1);
    }
}

RetFlag Par::parseOne(Toks*toks, ParDoc &doc, int level) {
    
    switch (matching) {
            
        case kMatchMeta:
        case kMatchRegx: return parseRegx(toks,doc,level);
            
        case kMatchWave: return parseWave(toks,doc,level);
        case kMatchQuo:  return parseQuo (toks,doc,level);
        case kMatchAnd:  return parseAnd (toks,doc,level);
        case kMatchOr:   return parseOr  (toks,doc,level);
    }
}

RetFlag Par::parseMny(Toks*toks, ParDoc &doc, int level) {
    
    int count = 0;
    
    for (RetFlag ret=kRetZero; ret!=kRetNope && doc.hasMore();) {
        
        switch (matching) {
                
            case kMatchMeta:
            case kMatchRegx: ret = parseRegx(toks,doc,level); break;
                
            case kMatchWave: ret = parseWave(toks,doc,level); break;
            case kMatchQuo:  ret = parseQuo (toks,doc,level); break;
            case kMatchAnd:  ret = parseAnd (toks,doc,level); break;
            case kMatchOr:   ret = parseOr  (toks,doc,level); break;
        }
        if (ret!=kRetNope) {
            count++;
            if (count==MaxCountDefault) {
                PrintParseTok("\n*** %s:parseMany exceeded maximum count:%i ***\n",name.c_str(),MaxCountDefault);
                return kRetNope;
            }
        }
    }
    return (count>0 ? kRetMatch : kRetNope);
}

RetFlag Par::parseAny(Toks*toks, ParDoc &doc, int level) {

    int count = 0;

    for (RetFlag ret=kRetZero; ret!=kRetNope && doc.hasMore();) {
        
        switch (matching) {
                
            case kMatchMeta:
            case kMatchRegx: ret = parseRegx(toks,doc,level); break;
            
            case kMatchWave: ret = parseWave(toks,doc,level); break;
            case kMatchQuo:  ret = parseQuo (toks,doc,level); break;
            case kMatchAnd:  ret = parseAnd (toks,doc,level); break;
            case kMatchOr:   ret = parseOr  (toks,doc,level); break;
        }
        if (ret!=kRetNope) {
            count++;
            if (count==MaxCountDefault) {
                PrintParseTok("\n*** %s:parseMany exceeded maximum count:%i",name.c_str(),MaxCountDefault);
                exit(-1);
                return kRetNope;
            }
       }
    }
    return (count>0 ? kRetMatch : kRetZero);
}

RetFlag Par::parseOpt(Toks*toks, ParDoc &doc, int level) {

    RetFlag ret;
    switch (matching) {
            
        case kMatchMeta:
        case kMatchRegx: ret = parseRegx(toks,doc,level); break;
            
        case kMatchWave: ret = parseWave(toks,doc,level); break;
        case kMatchQuo:  ret = parseQuo (toks,doc,level); break;
        case kMatchAnd:  ret = parseAnd (toks,doc,level); break;
        case kMatchOr:   ret = parseOr  (toks,doc,level); break;
    }
    switch (ret) {
        
        case kRetNope:  return kRetZero;
        case kRetZero:  return kRetZero;
        case kRetMatch: return kRetMatch;
        case kRetEnd:   return kRetEnd;
    }
}

#pragma mark - and or meta wave
/*
def (a ~ b ~ c ~ d) {
    
    doc        par            prior    tokens
    ⦙x c y z b⦙ (*a b c d e)
    ⦙x c y z b⦙ (a *b c d e)
    ⦙x c y z b⦙ (a b *c d e) => "x"
    
    ⦙c y z b c⦙ (*a b c d e)
    ⦙c y z b c⦙ (a *b c d e)
    ⦙c y z b c⦙ (a b *c d e) =>        (~,"x"),(c,"c")
    
    ⦙y z b c  ⦙ (*a b c d e)
    ⦙y z b c  ⦙ (a *b c d e)
    ⦙y z b c  ⦙ (a b *c d e) =>  "y"
    
    ⦙z b c    ⦙ (*a b c d e)
    ⦙z b c    ⦙ (a *b c d e)
    ⦙z b c    ⦙ (a b *c d e) =>  "y z"
    
    ⦙b c      ⦙ (*a b c d e)
    ⦙b c      ⦙ (a *b c d e)         (~,"x"),(c,"c"),(~,"y z"),(b,"b")
    
    ⦙c        ⦙ (*a b c d e)
    ⦙c        ⦙ (a *b c d e)
    ⦙c        ⦙ (a b *c d e)         (~,"x"),(c,"c"),(~,"y z"),(b,"b")(c,"c")
}
*/

RetFlag Par::parseWave(Toks*toks, ParDoc &doc, int level) {
    
    int count = 0;
    int startIdx = doc.idx; // start of unmatched string
    static string wave = "~    ";
    while (doc.hasMore()) {
        
        int frontIdx = doc.idx;
        
        for (Par *par : parList) {
            
            if (par->parse(toks, doc,level)==kRetMatch) {
                
                count ++;
                
                // insert prior unmatched words as a Tok("~wave",...)
                
                if (startIdx < frontIdx) {
                    int endIdx = doc.trimSpace(frontIdx);
                    Tok*unmatched = new Tok(wave,doc._chr,startIdx,endIdx,level+1);
                    
                    // insert unmatched before matched
                    Tok*matched = toks->back();
                    toks->pop_back();
                    toks->push_back(unmatched);
                    toks->push_back(matched);
                }
                startIdx = doc.idx;
                goto skip; // already advanced to enxt word
            }
        }
        doc.nextWord();
    skip:
        continue;
    }
    if (count>0 && startIdx < doc.idx) {
        
        Tok*unmatched = new Tok(wave,doc._chr,startIdx,doc.trimSpace(doc.idx),level+1);
        toks->push_back(unmatched);
    }

    return (count>0 ? kRetMatch : kRetNope);
}

/* capture all the words between matches
 */
inline RetFlag Par::parseMeta(Toks *toks, ParDoc&doc, int level, Par *&meta, Par*par) {
    
    // for b in '(a ~b c)'
    if (par->matching==kMatchMeta) {
        
        meta = par;
        if (meta==parList.back()) {
            for (int row = doc.row; row==doc.row && doc.hasMore();) {
                meta->parseRegx(toks, doc, level+1);
            }
        }
    }
    // for a in (a ~b c)
    else if (!meta) {
        return par->parse(toks, doc,level);
    }
    // for c in  (a ~b c?) or (a ~b c*)
    else /* (meta) */{
        
        int startIdx = doc.idx;
        int frontIdx = startIdx;

        for (int row = doc.row; row==doc.row && doc.hasMore();) {

            if (par->parse(toks, doc,level)==kRetMatch) {
                
                if (startIdx < frontIdx) {
                    
                    // prepare to insert meta before mateched token
                    Tok*matched = toks->back();
                    toks->pop_back();
                    int endIdx = doc.trimSpace(frontIdx);
                    if (meta->parList.size()) {
                        
                        char hack = doc.pushCutHack(frontIdx);
                        doc.idx = startIdx;
                        
                        for (Par*metai :meta->parList) {
                            metai->parse(toks, doc, level+1);
                        }
                        doc.idx = frontIdx;
                        doc.popCutHack(frontIdx, hack);
                    }
                    else {
                        Tok*unmatched = new Tok(meta->name,doc._chr,startIdx,endIdx,level+1);
                        
                        // insert unmatched before matched
                        toks->push_back(unmatched);
                    }
                    toks->push_back(matched);
                }
                return kRetMatch;
            }
            else if (doc.nextWord()) {
                frontIdx = doc.idx;
                continue;
            }
            else {
                meta = 0;
                return kRetMatch;
            }
        }
    }
    return kRetMatch;
}

RetFlag Par::parseAnd(Toks*toks, ParDoc &doc, int level) {
    
    PushTok
    
    Par *meta = 0;
    
    for (Par *par : parList) {
        
        switch (parseMeta(toks,doc,level,meta,par)) {
            
            case kRetNope:
            case kRetEnd: {
                
                PopTok
                return kRetNope;
            }
            case kRetZero:
            case kRetMatch: {
                
                break;
            }
        }
    }
    return kRetMatch;
}

RetFlag Par::parseOr(Toks*toks, ParDoc &doc, int level) {
    
    PushTok
    for (Par *par : parList) {
        
        RetFlag ret = par->parse(toks, doc,level);
        if (ret!=kRetNope) {
            return kRetMatch;
        }
     }
    PopTok

    return kRetNope;
}        

#pragma mark - parse leaf

/* match string 
 */
RetFlag Par::parseQuo(Toks*toks, ParDoc &doc, int level) {
    
    if (match.quo && match.quo->parse(doc)) {
        
        pushTok(toks,level,doc);
        return kRetMatch;
    }
    return kRetNope;
}

/* match regular expression 
 */
RetFlag Par::parseRegx(Toks*toks, ParDoc &doc, int level) {
    
    if (match.regx && match.regx->parse(doc)) {
        
        // anonymous regx is named "?"
        
        if (name=="?") {
            
            Tok*back = toks->back();
            *back->value =  (match.regx->result2.size() > 0
                             ? match.regx->result2 // use b in ('a'=>'b')
                             : match.regx->result);
        }
        else {
            
            printLevelInputMargin(level, doc);
            PrintParseTok("%s: %s\n", name.c_str(),(const char*)*match.regx);
            
            Tok*token = new Tok(name,(const char*)*match.regx,level);
            toks->push_back(token);
        }
        return kRetMatch;
    }
    return kRetNope;
}
