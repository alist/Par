
#import <unordered_map>
#import <deque>
#import <vector>
#import <string>

using namespace std;
typedef int TokType;
struct ParTok;
typedef vector<ParTok*> ParToks;
typedef unordered_map<string,TokType> ParNameEnum;   // translate token name string into enum type
typedef unordered_map<TokType,string> ParEnumName;   // translate token enum type into name string

