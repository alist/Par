
#import <unordered_map>
#import <deque>
#import <vector>
#import <string>

using namespace std;
typedef int TokType;
struct Tok;
typedef vector<Tok*> Toks;
typedef unordered_map<string,TokType> TokNameEnum;   // translate token name string into enum type
typedef unordered_map<TokType,string> TokEnumName;   // translate token enum type into name string

