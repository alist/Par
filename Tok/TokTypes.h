/* Copyright © 2015 Warren Stringer - MIT License - see file: License.mit */

#import <unordered_map>
#import <deque>
#import <vector>
#import <string>

/* allow "case str2int("name"):"
 * from http://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
 * The compiler will complain about duplicates for static collisions
 *
 * *** Warning *** There may be dynamic collisions during runtime
 * grammar with 9 characters or less should always be ok 
 * as this shifts 7 bits in a int64_t  words (9*7 => 63 bits)
 * there is an assert during debug to test for collisions.
 *
 * TODO: compare with Murmur3_32 -> is there a int64_t version?
 * Performance? Murmur3_32 has a lot more steps
 */
constexpr int64_t str2int(const char* str, int h = 0) {
    return !str[h] ? 0 : (str2int(str, h+1)<<7 | str2int(str, h+1)>>57) ^ str[h];
}

using namespace std;
typedef int64_t TokType;
struct Tok;
typedef vector<Tok*> Toks;
typedef unordered_map<string,TokType> TokNameEnum;   // translate token name string into enum type
typedef unordered_map<TokType,string> TokEnumName;   // translate token enum type into name string

