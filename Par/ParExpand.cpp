#import "ParExpand.h"
#import "Tok.h"

#define PrintParExpand(...) //DebugPrint(__VA_ARGS__)

#pragma mark - expand

void ParExpand::expandTokens(Toks*tokens_) {
    
    tokens = tokens_;
    expand(0);
}
inline bool ParExpand::thisTokIsSibling(int toki, int level) {
    
    if (toki < tokens->size() &&
        (*tokens)[toki]->level >= level) {
        return true;
    }
    else {
        return false;
    }
}

inline Tok *ParExpand::incrementToki(int &toki) {
    
    toki++;
    if (toki>=tokens->size()) {
        return 0;
    }
    Tok *tok = (*tokens)[toki];
    
//    if (tok->tokType==kTok_tr3) {
//        
//        tokStack.setStack(tok->level, toki);
//    }
//    else  {
         tokStack.setStack(tok->level);
//    }
    return tok;
}

int ParExpand::expand(int toki) {
    
    //int size  = tokens->size();
    Tok *tok =(*tokens)[toki];
    int level = tok->level;
    
    for (; thisTokIsSibling(toki, level);
         tok=incrementToki(toki)) {
        
        switch (tok->tokType) {
            
//            case kTok_path:  return expandPath(ttss, toki);
//            case kTok_name:  return expandName(ttss, toki);
//            case kTok_child: return expandMany(ttss, toki);
//            case kTok_dlist: return expandMany(ttss, toki);
//            case kTok_subt:  return expandMany(ttss, toki);
//            case kTok_array: return expandArray(ttss, toki);
//            case kTok_enums: return expandValue(ttss, toki);
//            case kTok_value: return expandValue(ttss, toki);
//            case kTok_input: return expandEdges(ttss, toki);
//            case kTok_output:return expandEdges(ttss, toki);
                
            default: continue;
        }
    }
    return toki;
}


/* either expand or find a name
 * point tr3PrevP to new tr3
 *
inline int ParExpand::expandName(TypeTr3ss *ttss, int toki) {
    
    Tok*tok = (*tokens)[toki]; //TODO: redundant, optimized away?
    TokType tokPrev = ttss->back()->type;
    
    // a:(b) or a(b) - find b
    if (tokPrev==kTok_dlist ||
        tokPrev==kTok_child ||
        tokPrev==kTok_subt) {
        
        Tr3* tr3 = Tr3Find::findTr3(ttss, ttss->size()-1, tok->value);
        
        if (tr3) {
            
            Tr3 *copy = deepCopy(tr3);
            copy->clone = true;
            copy->path = true;
            copy->tokType = tokPrev;
            copy->tokt = tokStack.getTokt();
            ttss->back()->tr3s->push_back(copy);
            PrintParExpand("&(%i:%s) ",toki, tok->value->c_str());
            return toki;
        }
    }
    //
    Tr3 *tr3 = new Tr3(tok->value->c_str());
    tr3->tokType = tokPrev;
    tr3->tokt = tokStack.getTokt();
    ttss->back()->tr3s->push_back(tr3);
    PrintParExpand("+(%i:%s) ",toki,tok->value->c_str());
    return toki;
}
*/
