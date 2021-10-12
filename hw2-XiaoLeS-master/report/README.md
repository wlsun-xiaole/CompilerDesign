# hw2 report

|||
|-:|:-|
|Name|孫汶琳|
|ID|0680707|

## How much time did you spend on this project

About 6 hours for coding and an hour for the report.

## Project overview

In this project, we build a parser for checking given grammars of input program with YACC tools.
Following the instructions provided in professor's lecture, we start the project from updating my scanner.
According to the lecture, while YACC checking the grammars of given codes, it repeatly ask LEX, specifically, the scanner, for the tokens.
We consider the `tokens` as the `terminals` we have learned on the parsing lectures.
As the result, the first work for us is to add the return value of each terminal, for example, the key words *FOR* (for loop), *BEGIN* (begin), *END* (end), etc.
Referring to the sample scanner code, TA moves the main function to the parser, so we remove the main function definition in the scanner and modify the sample parser code to our own naming (like the *Buffer* and *LineNum* used in the parser).
The following table shows part of return value in the scanner and their relative token declarations in the parser.

|||
|:-|:-|
|Scanner|Parser|
|"array"	{ token(KWarray); return ARRAY; }|%token ARRAY|
|"begin" 	{ token(KWbegin); return BEG; }|%token BEG|
|"boolean" 	{ token(KWboolean); return BOOL; }|%token BOOL|
|"def"		{ token(KWdef); return DEF; }|%token DEF|
|"do"		{ token(KWdo); return DO; }|%token DO|
|"else"		{ token(KWelse); return ELSE; }|%token ELSE|
|"end"		{ token(KWend); return END; }|%token END|

After these work, we can start our main part in this project: the scanner.


The YACC code structure is composed of three parts, an optional `declarations` part, a required `grammar rules` part and an optional `user subroutines` part.
```
[ declarations(optional)]

%{
	C declarations and includes
%}

%token <name1> <name2>
%start <symbol>

%%

[ grammar rules (required)]

%%

[ user subroutines (optional)]
```

The `declarations` includes some C declarations and header file includes, the tokens declarations, and the start symbol.
Following the lecture, we add the tokens declarations that we have done to the scanner.
Actually, in the scanner, we include a header file named `parser.h` (`y.tab.h` in the lecture).
In `parser.h`, YACC generates the token definitions (`#define token_name token_id`) in accordance to the token declarations we declared in our parser.
That is, with `parser.h` our scanner and parser unite the token declarations for their communication.
Therefore, refering to the return value we have add to the scanner, we add the token declarations.
At the end of this part, we add the start symbol to declare the root of the grammar that we are going to define in the `grammar rules` part.
In our project, we define it as `program`.

After the tokens are declared, we start to define the grammar of our program language.
The whole procedure is to follow the provided definitions one-by-one and implement them in the `grammar rules` part recursively.
For example, at the beginning, we found the `program` definition as following.
```
 identifier;
 <zero or more variable and constant declaration>
 <zero or more function declaration and definition>
 <one compound statement>
 end
```
The `identifier` (`ID`), `;` (`M_SEMICOLON`), and `end` (`END`) are tokens (terminals) that we have declared, and the three <...> includes the non-terminals (`list_var_const_decl`, `list_func_decl`, `a_stat_compound`) that we haven't defined.
We define the `program` as following.
```
program	:	ID M_SEMICOLON
			list_var_const_decl
			list_func_decl
			a_stat_compound 
			END
		;
```
Then, the next step is to find their definition provided by TA (or the testcases) and define them repeatly until every non-terminal is defined.

```
list_var_const_decl	:
					|	list_var_const_decl1
					;
list_var_const_decl1:	a_var_const_decl
					|	list_var_const_decl1
						a_var_const_decl
					;

...


list_func_decl	:	
				| 	list_func_decl1
				;
list_func_decl1 :	a_func_decl
				|	list_func_decl1
					a_func_decl
				;

...

a_stat_compound	:	BEG
					list_var_const_decl
					list_stat
					END
				;

...

```

The final part is the `user subroutines` part.
In this part, we declare our main routine and the function for print out the error messages of the parsing result with C language.
Most of the definition are provided by TA, so we only have to modify the naming of `LineNum` and `Buffer` to the naming we used in our own scanner.

## What is the hardest you think in this project

The hardest part in this project is debugging.
I use about an hour to find out the reason of building errors, and one and the most critical of these bugs is that my token definition of the keyword "begin".
Originally, I used *BEGIN* and it is already defined.
Since the error messages are not about re-defined or ambiguous, I used a lot of time to find it out.
Another bug spends me about half of an hour is due to my incorrect handling of some grammar rules that should be defined as "zero or more".
This kind of bugs is hard to be found, since wrong results of testcase usually happens in the following line.
After I unite my naming of rules, this bug can be checked and corrected easily.
Although I did not use any of the debugging tool or checking the generated result, I thought it may be critical for the following projects, so I should survey more about it after this project.

## Feedback to T.A.s

感謝助教們這次還是提供了測試資料讓我們很方便地做測試，這次作業中有一些grammar rules是spec裡面沒有特別提到的，所以要是沒有測試資料來輔助的話，我相信會有許多定義是被忽視的。（雖然印象中都是蠻直覺該有的，不過像我這種單純做spec翻譯機的人，就很容易忽視ＸＤ e.g. 沒有return type的function declaration）
雖然這次作業在debug上花了我相對多一點的時間，不過寫起來真的是特別的開心。
邊寫邊感覺好像正在寫考古題時的解題感覺一樣，蠻新奇的！接在期中考後寫起來特別有感！
就像我前一個部分所說的，因為沒有遇到比較大的問題，所以就沒有特別研究怎麼開debug模式之類的特別功能了（最大的重複定義問題靠所產生的.c檔就找出來了），不過感覺之後專題中有可能需要，所以這部分可能還是需要另外花時間去玩玩看！
感謝老師和助教們的用心！期待下次的作業！

> For consecutive operation handling, I just simply exclude the state that may cause, for example `a--1`, `--1`, `not not`.
> I'm not sure the clear definition of consecutive operation is like what, so I just remove the possible state that TA has shown.
> Maybe it is better to provide a clear definition for the things that is included in grading policy. 

