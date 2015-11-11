#import "Par.h"
#import "Tok.h"
#import <stdarg.h>
#import <stdio.h>

#import "DebugPrint.h"

#define PrintParseTok(...) Debug(if (Par::Trace) { DebugPrint(__VA_ARGS__); })

int  Par::MemoNow=0;
bool Par::Trace=false;
int  Par::MaxCountDefault = 99; // max matches for a* or a+
int  Par::MaxLevelDefault =200; // max levels deep into parse tree

void Par::add(Par*n) {
    
    parList.push_back(n);
}

void Par::init() {
    
    name = "?";
    minCount = 1;
    maxCount = 1;
    matching = kMatchAnd;
    match.regx = 0;
    memoMe = -1;
}

void Par::init(RepeatType repeat_) {
    
    init();
    switch (repeat_) {
        case kRepOne: setMinMax(1, 1); break;
        case kRepAny: setMinMax(0,-1); break;
        case kRepMny: setMinMax(1,-1); break;
        case kRepOpt: setMinMax(0, 1); break;
    }
}
void Par::setMinMax(int minCount_, int maxCount_) {
    minCount = minCount_;
    maxCount = maxCount_>0 ? maxCount_ : MaxCountDefault;
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

void Par::setName(const char*name_) {
    name = name_;
}

Par& Par::operator =(Par&p_)  {
    
    //name     = p_.name; // do not copy name
    minCount   = p_.minCount;
    maxCount   = p_.maxCount;
    parList    = p_.parList;
    matching   = p_.matching;
    match.regx = p_.match.regx;
    match.quo  = p_.match.quo;
    return *this;
}

Par &Par::operator [](int index) {
    
    return *parList[index];
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

inline void Par::popTok(Toks*toks, int priorTokIdx) {
    
    while (toks->size()>priorTokIdx) {
        
        Tok*token = toks->back();
        delete (token);
        toks->pop_back();
    }
}

#pragma mark - parse

bool Par::parse(Toks*toks, ParDoc &doc, int level) {

    int count = 0;
    
    for (bool parsing=true; parsing && count<maxCount && doc.hasMore();) {
        
        switch (matching) {
                
            case kMatchAhead:
            case kMatchRegx: parsing = parseRegx(toks,doc,level+1); break;
            
            case kMatchWave: parsing = parseWave(toks,doc,level+1); break;
            case kMatchQuo:  parsing = parseQuo (toks,doc,level+1); break;
            case kMatchAnd:  parsing = parseAnd (toks,doc,level+1); break;
            case kMatchOr:   parsing = parseOr  (toks,doc,level+1); break;
        }
        if (parsing) {
            count++;
        }
    }
    if (count==MaxCountDefault) {
        PrintParseTok("\n*** %s:parseMany exceeded maximum count:%i",name.c_str(),MaxCountDefault);
        exit(-1);
    }
    return count>=minCount && count<=maxCount;
 }

#pragma mark - and or ahead wave

/* After matching c, match b in (a ~b c)
 */
inline void Par::parseBehind(Toks *toks, ParDoc&doc, int level, Par *&before, int &befIdx, int startIdx, int endIdx) {
    
    if (startIdx < doc.front) {
        
        endIdx = doc.trimSpace(endIdx);
       
        Tok *prevTok = toks->back();
        if (prevTok->level==level) {
             toks->pop_back();
        }
        if (before->parList.size()) {
            
            // overwrite a temporary '\0' to parse substring
            ParDoc cut; cut.copyState(doc);
            char hack = doc.pushCutHack();
            doc.idx = startIdx;
            
            for (Par*beforei : before->parList) {
                beforei->parse(toks, doc, level+1);
            }
            // reinstate full string from '\0'
            cut.popCutHack(hack);
            doc.copyState(cut);
        }
        else {
            
            Tok*unmatched = new Tok(before->name,doc._chr,startIdx,endIdx,level);
            toks->push_back(unmatched);
        }
        if (prevTok->level==level) {
            
            toks->push_back(prevTok);
        }
    }
}
/* match a,c in (a c), (a ~b c), ...
 */
inline bool Par::parseAhead(Toks *toks, ParDoc&doc, int level, Par *&before, int &befIdx, Par*par) {
    
    // for b in '(a ~b c)'
    if (par->matching==kMatchAhead) {
        
        before = par;
        befIdx = (int)toks->size();
        
        // for b in (a ~b)
        if (before==parList.back() && doc.hasMore()) {
            
            printLevelInputMargin(level, doc);
            PrintParseTok("%s: %s\n", before->name.c_str(),doc.ptr());
            
            Tok*token = new Tok(before->name,doc.ptr(),level);
            toks->push_back(token);
        }
    }
    // for a in (a ~b c)
    else if (!before) {
        
        return par->parse(toks, doc,level);
    }
    // for c in  (a ~b c?), were before==b
    else {
        
        int startIdx = doc.idx;

        while (doc.hasMore()) {
            
            int endIdx = startIdx;
            
            if (par->parse(toks, doc, level)) {
                
                parseBehind(toks, doc, level+1, before, befIdx, startIdx, endIdx);
                return true;
            }
            else if (doc.nextWord()) {
                continue;
            }
            else {
                before = 0;
                return true;
            }
        }
    }
    return true;
}
/* match a,b,... in  (a b)
 */
bool Par::parseAnd(Toks*toks, ParDoc &doc, int level) {
    
    // push doc and toks
    ParDoc prev; prev.copyState(doc);
    int priorTokIdx = pushTok(toks,level,doc);
    
    Par *before = 0;
    int befIdx;
    
    for (Par *par : parList) {
        
        if (!parseAhead(toks,doc,level,before,befIdx,par)) {
            
            doc.copyState(prev);        // pop doc
            popTok(toks,priorTokIdx);   // pop toks
            
            return false;
        }
    }
    return true;
}

/* match a, b, ... in (a | b)
 */
bool Par::parseOr(Toks*toks, ParDoc &doc, int level) {
    
    // push doc and toks
    ParDoc prev; prev.copyState(doc);
    int priorTokIdx = pushTok(toks,level,doc);
    
    for (Par *par : parList) {
        
        if (par->parse(toks, doc,level)) {
            return true;
        }
    }
    doc.copyState(prev);        // pop doc
    popTok(toks,priorTokIdx);   // pop toks
    
    return false;
}

#pragma mark - parseWave Islands

/* parseWave looks of islands of matches by scanning the document 
 * word-by-word until one of the patterns is matched and repeat.
 * Any unmatched portions between are put in a '~' token. 
 * If no matches, then the document backtracks to the beginning.
 
 example: {
 
    def (a ~ b ~ c ~ d)
    a('^a')
    b('^b')
    c('^c')
    d('^d')
    
with input string
    
    "x c y z b"
    
resulting in the token stream
    
    0, 2:  a : a
    1, 2:  ~ : x
    2, 2:  c : c
    3, 2:  ~ : y z
    4, 2:  b : b
    
here is the word-by-word parse of the input stream
    
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
bool Par::parseWave(Toks*toks, ParDoc &doc, int level) {
    
    int count = 0;
    int startIdx = doc.idx; // start of unmatched string
    static string wave = "~";
    
    while (doc.hasMore()) {
        
        for (Par *par : parList) {
            
            if (par->parse(toks, doc,level)) {
                
                count ++;
                
                // insert prior unmatched words as a Tok("~   ",...)
                
                if (startIdx < doc.front) {
                    
                    int endIdx = doc.trimSpace(doc.front);
                    Tok*unmatched = new Tok(wave,doc._chr,startIdx,endIdx,level+1);
                    Tok* prevTok = toks->size() ? toks->back() : 0;
                    
                    if (!prevTok || prevTok->level!=unmatched->level) {
                        toks->push_back(unmatched);
                    }
                    // insert unmatched before matched
                    else {
                        Tok*prevTok=toks->back();
                        toks->pop_back();
                        toks->push_back(unmatched);
                        toks->push_back(prevTok);
                    }
                }
                startIdx = doc.idx;
                goto skip; // already advanced to enxt word
            }
        }
        doc.nextWord();
    skip:
        continue;
    }
    if (count>0 && startIdx < doc.idx ) {
        
        int endIdx = doc.trimSpace(doc.idx);
        if (startIdx <= endIdx) {
            
            Tok*unmatched = new Tok(wave,doc._chr, startIdx, endIdx, level+1);
            toks->push_back(unmatched);
        }
    }
    return (count>0);
}

#pragma mark - parse leaf

/* match literal a in ("a" b)
 */
bool Par::parseQuo(Toks*toks, ParDoc &doc, int level) {
    
    if (match.quo && match.quo->parse(doc)) {
        
        printLevelInputMargin(level, doc);
        pushTok(toks,level,doc);
        return true;
    }
    return false;
}

/* match regular expression a in ('^a' b)
 */
bool Par::parseRegx(Toks*toks, ParDoc &doc, int level) {
    
    if (match.regx && match.regx->parse(doc)) {
        
        // anonymous regx is named "?"
        
        if (name=="?") {
            if (toks->size()) {
                Tok*back = toks->back();
                *back->value =  (match.regx->result2.size() > 0
                                 ? match.regx->result2 // use b in ('a'=>'b')
                                 : match.regx->result);
            }
        }
        else {
            
            printLevelInputMargin(level, doc);
            PrintParseTok("%s: %s\n", name.c_str(),(const char*)*match.regx);
            
            Tok*token = new Tok(name,(const char*)*match.regx,level);
            toks->push_back(token);
        }
        return true;
    }
    return false;
}
