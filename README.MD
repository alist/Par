# Par 

Par extends regular expressions to create a token stream. It simplifies bnf with a regex like alternation and repetition:

    a (b (c1|c2)? d+ e*)+

Two oddities include: look ahead and island parsing: 

    (a ~b c) // which matches b only if c is matched
    (a ~ c) // which tries to match a and c in parallel

Par is dynamic; it compiles the grammar during runtime. The goal is to run NLP parsers and domain specific languages locally on mobile devices. In many cases, the grammar can be updated via live update without needing to recompile the runtime binary.

### Example

Check out `ParTest.pdf` as it describes how `ParTest.cpp` creates a grammar and processes tokens to do something useful. 

### Syntax Walkthrough

Par extends regular expressions to produce a token stream
that includes: tokId, level, name, and optional value. For example, the grammar
    yo (hello world)
    hello ('hello')
    world ('world')
    
will convert this string `"hello world"` into the following token stream

    0, 1: yo :
    1, 2:  hello : hello
    2, 2:  world : world

The top name can be anything, `yo`, `oy`, whatever you want. \
Par supports alternation `|` and repetition:
```c
    + {1,}  // 1 or more
    * {0,}  // 0 or more
    ? {0,}  // zero or 1
      {2}   // exactly 2
     {2,3}  // 2 or 3
      {,3}  // up to 3
```
So, changing def to:
```c
    def (hello | world)+
    hello ('hello')
    world ('world')
```
changes the output stream `hello world` to two defs:
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 1: def :
    3, 2:  world : world
```
Par uses PCRE to match everything between the `'…'` quotes.
So, the regex patterns behave as expected, including:\
    `^` to match at the beginning ('^first')\
    `$` anchor to end of line ('last$')

plus all of the standard quantifiers `*+?{}` So, the grammar:
```c
    def (hello | world)+
    hello ('he?ll?o')
    world ('wo?rld$')
```
will match `hlo`, ignore `yo` and match `wrld`
```c
    0, 1: def :
    1, 2:  hello : hlo
    2, 1: def :
    3, 2:  world : wrld
```
When Par matches a pattern it advances the document pointer past the match to the next non-whitespace character. Thus, anchoring with `^` is crucial. 

For example, swapping the order of world and hello, without an anchor:
```c
    def (world | hello)+ // will search for 'world' first
    hello ('hello')
    world ('world')
```
`"hello world"` will skip hello, resulting in only one match
```c
    0, 1: def :
    1, 2:  world : world
```
To fix, anchor with `^`
```c
    def (world | hello)+
    hello ('^hello')
    world ('^world')
```
`"hello world"` -- will yield independent def results
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 1: def :
    3, 2:  world : world
```
There is also an option to `(n'…')` to not advance cursor.
```c
def (hello world)
hello (n'^hello') // find at beginning without moving cursor
world (n'world$') // find world at and without moving cursor
```
`"hello big beautiful world"` -- which ignores everything in between
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 2:  world : world
```
To capture everything everthing between, use '~name' which looks ahead to see if the following pattern matches. So:
```c
    def (hello ~middle world)
    hello ('^hello')
    world ('^world')
```
`"hello big and beautiful world"` -- will result in
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 2:  ~middle : big beautiful
    3, 2:  world : world
```
Above, `~middle` didnt match subsequent sub-grammar To add a sub-search, define a `middle`:
```c
    def (hello ~middle world)
    hello ('^hello')
    world ('^world')
    middle ('big' | 'beautiful')+ // middle does a subsearch
```
    0, 1: def :
    1, 2:  hello : hello
    2, 4:    ~middle : big
    3, 4:    ~middle : beautiful
    4, 2:  world : world

Another option is `(i'…')` to match independent of case:
```c
    def (hello world)+
    hello (i'hello')
    world (i'world')
```
`"Hello WORLD"` -- which will match the mixed case string
```c
    0, 1: def :
    1, 2:  hello : Hello
    2, 2:  world : WORLD
```
Par doesn't require a surrounding parens `(…)`. But, is sometimes worthwhile to have an inner set of parens:
```c
    def (hello world)+
    hello ('h(el)?lo') // here to match either hello or hlo
    world ('world')
```
`"hello world"` --  will return a confusing result:
```c
    0, 1: def :
    1, 2:  hello : el
    2, 2:  world : world
```
So, add surrounding parens
```c
    def (hello world)+
    hello ('(he(ll)?o)') // here
    world ('world')
```
`"hello world"` -- to insure that the full input is returned
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 2:  world : world
```
Par also matches string literals between double quotes `"..."`
Which comes in handy for parsing special characters that may be less readable as a regex: such as `()[]{}!$^*?` .
```c
    def (hello world paren? emoji?)+
    hello ('hello')
    world ('world')
    paren ("(" inside ")")
    inside (i'[a-z ]*')
    emoji('^😊|😃|😏|😈')
```
`"hello world (yo) 😊"` -- resulting in the following stream
```c
    0, 1: def :
    1, 2:  hello : hello
    2, 2:  world : world
    3, 2:  paren :
    4, 3:   inside : yo
    5, 2:  emoji : 😊
```
Notice that `paren(…)` combined literals and symbols. If the values are not needed, we could go crazy and combine everything together in a compound statement like, so:
```c
    woohoo ('hello' 'world' ("(" i'[a-z ]*' ")")? '^😊|😃|😏|😈')+
    "hello beautiful world (yo) 😊" // resulting in a single token
     0, 1: woohoo : 😊
```
### Recursion

Right recursion is allowed; as long as there is preceding pattern
that matches one more times
```c
    def (hello def? world)+
    hello (i'^hello'|i'(good (morning|afternoon|evening))')
    world (i'^world'|i'^planet'|i'^earth')
```
`"Hello good morning planet earth"` -- yields:
```c
    0, 1: def :
    1, 2:  hello : Hello
    2, 2:  def :
    3, 3:   hello : good morning
    4, 3:   world : planet
    5, 2:  world : earth
```
Left recursion will exit the app:
```c
    def (def? hello world)  // or
    def (hello* def? world) // for
```
`"hello world"` -- will result in the following message
```c
    *** def:parse exceeded maximum levels: 200 ***
```
### Island Parsing
The wave '~' (tilde) searches in parallel. For Example:
```c
    def (aloha ~ hello ~ world ~ close)
    aloha (i'^aloha')
    hello (i'^hello'|i'^(good (morning|afternoon|evening))')
    world (i'^world'|i'^planet'|i'^earth')
    close (i'^yours truly')
```
Given the following input document
```c
"Yo! Good morning big beautiful planet earth - Aloha from yours truly."
```
Produces the following token stream
```c
    0, 2:  ~ : Yo!
    1, 2:  hello : Good morning
    2, 2:  ~ : big beautiful
    3, 2:  world : planet
    4, 2:  world : earth
    5, 2:  ~ : -
    6, 2:  aloha : Aloha
    7, 2:  ~ : from
    8, 2:  close : yours truly
    9, 2:  ~ : .
```
The `~` tokens are the unmatched portions of the document -- the water between the islands. Par will advance word by word, and try all the patterns until
a match is found.

## GOTCHA - LookAhead between literals

Literal quotes treat everthing that is not a white space as part of word, which is fine for left to right matching like so:
```c
    paren ("(" inside ")") // this matches
    inside (i'[a-z ]*')
```
`"yo"` -- will produce the input stream:
     1, 2:  inside : yo
whereas lookahead between quotes will fail
```c
    paren ("(" ~inside ")") // this doesn't match
```
After matching the first `"("` the lookahead for `")"` tries to match `"yo)"` -- here is a trace:
```c
    ⦙(yo)     ⦙   1: paren
    ⦙yo)      ⦙   2:   0, 1: paren :
    // no tokens were produced
```
To fix, use a regex with initial anchor `'^('` and unanchored `')'`
```c
    paren ('^\(' ~inside '\)') 
```
producing the token stream
```c 
    0, 1: paren :
    1, 4:    ~inside : yo
```

## Files
```c
Par/
Par.*       - apply grammar to input to create token stream
ParPar.*    - convert *.par file into grammar
ParDoc.*    - manage an input document with shared buffer
ParQuo.*    - match a literal string
ParRegx.*   - match a regex pattern
Par.boot    - bootstrap macros that define the Par parser
Par.par     - informal description of Par syntax
ParBoot.h   - convert macros to Par grammar, used only for bootstrap

Tok/
Tok.*       - token item produced by parse
TokTypes.h  - types shared by all token items
TokStack.*  - utility for managing token hierachy backtracking

Test/
ParTest.cpp     - tests - also a working example of how to use Par
TestSuite.par   - parser grammar to read and run Par tests
TestSuite.test  - set of test examples to run and verify
<name>.par      - parser grammar file
<name>.txt      - test source example for <name>.par

pcre/           - a Perl-Compatible Regular Expression parser
```
## TODO

Memory sweep -- any leaks?\
Compiler settings -- guard malloc, xcode user certificats

## WISH LIST 

Command line - par, input, output, compare, trace, man pages\
Pod file\
RxCPP, RxSwift, RxEtc wrappers

## License

The MIT License (MIT)

Copyright © 2015 Warren Stringer - linkable MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

A reference to the above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
