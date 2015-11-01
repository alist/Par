
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

