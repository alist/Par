/* Copyright © 2015 Warren Stringer - MIT License - see file: license.mit */

#import "Par.h"
#import "Tok.h"
#import <stdarg.h>
#import <stdio.h>

#import "DebugPrint.h"

#define PrintParseTok(...) Debug(if (Par::Trace) { DebugPrint(__VA_ARGS__); })

int  Par::MemoNow=0;            // current global memoize id - increase for every new document
bool Par::Trace=false;          // show a sliding window of input doc while tokenizing
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

#pragma mark - push pop tok

/* Par adds tokens to toks while matching and backtracks
 * to previous position in toks if the match fails
 */
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

inline void Par::popTok(Toks*toks, int priorToki) {
    
    while (toks->size()>priorToki) {
        
        Tok*token = toks->back();
        delete (token);
        toks->pop_back();
    }
}

#pragma mark - parse

bool Par::parseStart(Toks*toks, ParDoc &doc) {

    int level = 0;
    Par *before = 0;
    doc.eatWhitespace();
    doc.trimTrailspace();
    return parse(toks,doc,level,before);
}

inline bool Par::parse(Toks*toks, ParDoc &doc, int level, Par*&before) {

    int count = 0;
    
    for (bool parsing=true; parsing && count<maxCount && doc.hasMore();) {
        
        switch (matching) {
                
            case kMatchAhead:
            case kMatchRegx: parsing = parseRegx(toks,doc,level+1); break;
            case kMatchQuo:  parsing = parseQuo (toks,doc,level+1); break;
            
            case kMatchWave: parsing = parseWave(toks,doc,level+1,before); break;
            case kMatchAnd:  parsing = parseAnd (toks,doc,level+1,before); break;
            case kMatchOr:   parsing = parseOr  (toks,doc,level+1,before); break;
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
 * The '~b' may refer to a corresponding statement 'b(...)'
 * or '~b' may be without a corresponding statement.
 */
inline void Par::parseBefore(Toks *toks, ParDoc&doc, int level, Par *&before, int startIdx, int endIdx) {
    
    if (startIdx < endIdx) {
        
        endIdx = doc.trimSpace(endIdx);
       
        // for b with a corresponding Par b(...)
         if (before->parList.size()) {
             
            // overwrite a temporary '\0' to parse substring
            ParDoc cut(doc);
            char hack = doc.pushCutHack(endIdx);
            doc.idx = startIdx;
            
            for (Par*beforei : before->parList) {
                // parse will add 1 back to ~name
                beforei->parse(toks, doc, level-1, before);
            }
            // reinstate full string from '\0'
            cut.popCutHack(endIdx,hack);
            doc = cut;
        }
        // for b without corresponding b(...)
        else {
            
            Tok*unmatched = new Tok(before->name,doc._chr,startIdx,endIdx,level);
            toks->push_back(unmatched);
        }
    }
}

/* match a,c in (a c), (a ~b c), ...
 */
inline bool Par::parseAhead(Toks *toks, ParDoc&doc, int level, Par *&before, Par*par) {
    
    // for b in '(a ~b c)'
    if (par->matching==kMatchAhead) {
        
        before = par;
        
        // for b in (a ~b)
        if (before==parList.back() && doc.hasMore()) {
            
            printLevelInputMargin(level, doc);
            PrintParseTok("%s: %s\n", before->name.c_str(),doc.ptr());
            
            Tok*token = new Tok(before->name,doc.ptr(),level);
            toks->push_back(token);
            before = 0;
        }
    }
    // for a in (a ~b c)
    else if (!before) {
        
        return par->parse(toks, doc,level, before);
    }
    // for c in  (a ~b c?)
    else {
        
        int startIdx = doc.idx;
        size_t tokBefi = toks->size();
        
        while (doc.hasMore()) {
        
            int endIdx = doc.idx;
            
            // parse c (or c?)
            if (par->parse(toks, doc, level, before) &&
                toks->size()>tokBefi) // did c? actually match something
            {
                
                // detach new after tokens from toks
                Toks after;
                auto befi = std::next(toks->begin(), tokBefi);
                after.insert(after.end(),befi, toks->end());
                toks->resize(tokBefi);
                
                // insert before tokens
                parseBefore(toks, doc, level+1, before, startIdx, endIdx);
                
                // add back the new tokens
                toks->insert(end(*toks), begin(after), end(after));
                before = 0;
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
inline bool Par::parseAnd(Toks*toks, ParDoc &doc, int level, Par *&ignore) {
    
    // push doc and toks
    ParDoc pushDoc(doc);
    int pushToki = pushTok(toks,level,doc);
    
    Par *before = 0;
    
    for (Par *par : parList) {
        
        if (!parseAhead(toks,doc,level,before,par)) {

            // pop doc and toks
            doc = pushDoc;
            popTok(toks,pushToki);
            
            return false;
        }
    }
    return true;
}

/* match a, b, ... in (a | b)
 */
inline bool Par::parseOr(Toks*toks, ParDoc &doc, int level, Par *&before) {
    
    // push doc and toks
    ParDoc pushDoc(doc);
    int pushToki = pushTok(toks,level,doc);
    
    for (Par *par : parList) {
        
        if (par->parse(toks, doc,level, before)) {
            return true;
        }
    }
     // pop doc and toks
    doc = pushDoc;
    popTok(toks,pushToki);
    
    return false;
}

#pragma mark - parseWave Islands

/* parseWave looks for islands of matches by scanning the document
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
inline bool Par::parseWave(Toks*toks, ParDoc &doc, int level, Par *&ignore) {
    
    // push doc and toks
    ParDoc pushDoc(doc);
    int pushToki = pushTok(toks,level,doc);

    
    int count = 0;
    int startIdx = doc.idx; // start of unmatched string
    int endIdx = startIdx;
    static string wave = "~";
    
    Par *before = 0;
    
    while (doc.hasMore()) {
        
        for (Par *par : parList) {
 
            endIdx = doc.idx;
            size_t tokBefi = toks->size();

            if (par->parse(toks, doc, level, before) &&
                toks->size()>tokBefi) {
                
                count ++;
                
                // insert prior unmatched words as a Tok("~",...)
                
                if (startIdx < endIdx) {
                    
                    endIdx = doc.trimSpace(endIdx);
                    Tok*unmatched = new Tok(wave,doc._chr,startIdx,endIdx,level+1);
                    auto befi = std::next(toks->begin(), tokBefi);
                    toks->insert(befi, unmatched);
                }
                startIdx = doc.idx;
                goto skip; // already advanced to enxt word
            }
        }
        doc.nextWord();
    skip:
        continue;
    }
    if (count==0) {
        // pop doc and toks
        doc = pushDoc;
        popTok(toks,pushToki);

    }
    else if (startIdx <= doc.idx ) {
        
        if (startIdx <= endIdx) {
            // trim back preceeding whitespace, unless this is a single character
            if (startIdx<endIdx) {
                endIdx = doc.trimSpace(endIdx);
            }
            Tok*unmatched = new Tok(wave,doc._chr, startIdx, endIdx, level+1);
            toks->push_back(unmatched);
        }
    }
    return (count>0);
}

#pragma mark - parse leaf

/* match literal a in ("a" b)
 */
inline bool Par::parseQuo(Toks*toks, ParDoc &doc, int level) {
    
    if (match.quo && match.quo->parse(doc)) {
        
        //printLevelInputMargin(level, doc);
        
        pushTok(toks,level,doc);
        return true;
    }
    return false;
}

/* match regular expression a in ('^a' b)
 */
inline bool Par::parseRegx(Toks*toks, ParDoc &doc, int level) {
    
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
