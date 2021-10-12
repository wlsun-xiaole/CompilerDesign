# hw3 report

|||
|-:|:-|
|Name|孫汶琳 Sun, Wen-Lin|
|ID|0680707|

## How much time did you spend on this project

5 days, about 50 hours.

## Project overview

In this project, we have to build up an abstract syntax tree (AST).
We describe the main role that is played by the scanner and the parser, respectively.

### Scanner
Since most of the scanner's work has been done in previous assignments, in this project, it only has to transform the literal value to specific data type.
And then the scanner stores the value to the `yylval`, in order to pass to the parser.
The required data tpyes are defined in the `%union {}` of the parser.
Therefore, if the scanner has transformed a constant integer `15` from literals, it can store the integer to `yylval.integer`.
(Assume that the union in the parser has the definition `%union{ long integer; }`.) 

### Parser
We have defined the value that passed by scanner, so we should do the `%type` define to make the connection between the data type and the tokens.
Also, besides the data type that required to get value from scanner, the data type of the tokens that used only in the parser should be defined in `%union` and connected by `%type`.
The data type in the `%union` can only be POD type.
That is, if you want to use the data type you defined, such as self-defined class and structure, you should define its pointer in the `%union`.
The nodes should be defined comprehensively in accordance to the AST we want to build up.

After the type of nodes are defined, we can start to write the actions that should be done while doing the reduction of parsing.
According to the lecture, we know that the action between the rules are just like to add a non-terminal to the rules and that is not what we want.
Since the rules are well defined, our actions should be only done during reductions.
The action of the `Program` is shown
```
Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1, "void", $3, $4, $5);

        free($1);
    }
;
```
In this example, we can found the action (between the `{}`) are defined at the end of the grammar rule.
The action is writtent in *C++* with some special syntaxes (`@` and `$`).
`@1` is used to access the data member defined in the `struct YYLTYPE` of the first token (`ProgramName` in this example).
Otherwise, `$1` represents the token type that defined in `%type` of the first token.
Our work is to make sure that all the tokens are defined and well mantained, so the parser can generate the code for use without ambiguity.

To handle different type of nodes, we use different self-defined classes.
Follow the previous `Program` example, we have to defined the classes `ProgramNode`, `DeclarationNode`, `FunctionNode`, and `CompoundStatementNode`.
(In my implementation, the `XXXList` part is done by using the standard container `vector` of the class, so I can focus on the implementation of each class.)
We can follow the descriptions in `ast_guideline.md`, to define the data members and methods of the self-defined classes.
Since I use the Visitor Pattern (VP) in this project, there are some special methods that I have to defined such as `accept()`.
The `print()` method in each classes from the template are not used and the `AstDumper` are used instead.
The `AstDumper` is used to traverse the AST we built with the help of the method `visitChildNodes` in each classes and print out the information of each node with required format.


## What is the hardest you think in this project
I think the hardest parts in this project are done by TA.
Thanks to TA's good naming, parser structure and class structure, the hardest part for me is just to make myself be familiar to the template and the *C++* language.
In the first three days, I built up the classes: `ProgramNode`, `DeclarationNode`, `VariableNode` and `ConstantValueNode`.
After that, I built up the classes: `FunctionNode`, `CompoundStatementNode`, `PrintNode` and some other classes that used by them in a day.
Finally, I finished up the rest of the statement nodes.
Just like TA mentioned on the lecture, the beginning part is pretty hard.

## Feedback to T.A.s
感謝助教的出色的template才讓我這次作業得以順利完成，跟著template, guideline和testcase走讓我覺得我又把C++複習了一次，前幾次的作業中沒用到的gdb也終於派上用場了，少了前述的幾項工具我想我到現在可能只能用C硬寫出一個我下次作業會想直接捨棄的作品吧...很高興這次的我一開始就決定直接使用助教的parser，雖然當初是因為想說VP不熟悉才下這樣的決定的，但也因此而欣賞到了相當出色的範例程式，算是意外之喜！邊寫邊把我遺失的C++給撿回來，或許還比以前更進步了也說不定，總而言之，雖然這次作業真的花了我好多時間，但真的收穫滿滿！感謝助教們和老師的用心！
在此特別感謝助教們迅速的問題排除和解惑，不然初期寫到快崩潰的我真的...會崩潰XD

P.S. 雖然不忍說，但我把助教的code改的好醜啊...因為懶(?)很多地方直接用vector包來取代掉判斷，然後expression要visit的時候也是直接把Down casting寫在visitChildNodes裡（等於每個要輸出ExpressionNode的都要複製貼上一次，很可怕），蠻期待下次作業能看到助教的範例程式的ＸＤ