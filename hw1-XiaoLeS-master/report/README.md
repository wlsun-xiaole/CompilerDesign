# hw1 report

|Field|Value|
|-:|:-|
|Name|孫汶琳 Sun, Wen-Lin|
|ID|0680707|

## How much time did you spend on this project

About 8 hours.

## Project overview

In this project, we built up a scanner according to the provided lexical definition of the `P` language.
Based on the template provided by TA, it is easy for us to get started.
In the remaining parts of this section, we describe the structure and relative implementation that we have done.

### The Structure of Scanner
The scanner is composed of three parts: definitions (C-language and Lex), rules, and routines.
```
%{
	// C-language definitions (include libraries, macro)
%}
	// Lex definitions (pre-defined rules or exclusive start conditions)
%%
	// rules (regular expressions and corresponding actions)
%%
	// routines (main function)
```
#### Definitions Section
The C-language definitions part includes the extra libraries and definitions we used. 
For example, we want to use `atoi()` in the standard library `stdlib.h` to convert string into integer for the actions defined in the rules section, so we use the C instruction `#include<stdlib.h>` to include the library.
```
%{
	#include<stdlib.h>
	...
%}
```
The Lex definitions part includes the pre-defined rules to simply the expression used in rules section.
Also, the exclusive start conditions are defined in this section, between the `%}` and the `%%` in the beginning of the rules section.
The exclusive start conditions section provide the extra state for the expression of the rules, such as the implementation of C/C++-style comments in this project uses two exclusive start conditions respectively.
```
	...
%}
integer 0|[1-9][0-9]*
string \"((\"\")|[^\"\n])*\"
	...
%x COMMENT_C
%x COMMENT_CPP

%%
```
#### Rules Section
The rules section defines the regular experssions and their corresponding actions.
In our implementation, the most of the space in this section is used to define the tokens and their actions.
The tokens are defined in regular expressions and their actions are written in C language.
In this project, we only have to make sure the tokens are recognized correctly, so most of actions of the tokens are to print relative text on the screen.
Some special tokens are required to do more actions on it, for example, the string constants.
In this example, we have to split out the content part between the double quotes without filtering out the double quotes that belongs to the string contents.
```
{string}	{
				LIST;
				if (Opt_T) {
					strBuf[0] = '\0';
					int wrt_cnt = 0, i;
					int twice = 0;	
					for (i=1; yytext[i]!='\0'; i++) {
						if (yytext[i] == '\"' && !twice) {
							twice = ~twice;
						} else {
							strBuf[wrt_cnt++] = yytext[i];
							twice = 0;
						}
					}
					strBuf[wrt_cnt] = '\0';
					printf("<%s: %s>\n", "string", strBuf);
				}
			}
```
Another example is the C++-style comments rules.
In this example, we define the `//` to begin the start condition `COMMENT_CPP` that we have defined in the definitions section.
After that we can easily use the `COMMENT_CPP` state to help us for defining the rules. 
```
"//"	{ LIST; BEGIN COMMENT_CPP; }
<COMMENT_CPP>[^\n]+	{ LIST; }
<COMMENT_CPP>"\n"	{
						LIST;
						if( Opt_S )
							printf("%d: %s", linenum, buf);
						linenum++;
						buf[0] = '\0';	
						BEGIN INITIAL;
					}
```

#### Routines section
The routines section is used to define the routines, including the main routine and some sub-routines that we define on our own.
We didn't modify this section in this project, since the main routine is given and we didn't define any sub-routine.

## What is the hardest you think in this project

The hardest part for me is to realize the syntax and the meaning of the sections that I have mentioned in the last question.
After realizing them, it is chill to follow the specification and write it out.
I have to say that this project is really helpful for us to understand the lectures.

## Feedback to T.A.s

（打到後來才想到這不是英文授課，所以我這部分就用中文打了。）
幾年前我有修過這門課，不過當時我沒有太多的時間和心力能用在這門課上所以就退選了; 當時交作業的模式和現在不一樣，助教給的也是一個template但沒有測試資料，對當時的我來說相當吃力，畢竟寫一寫感覺對了交出去很開心，但是看到成績就很難過，即使作業一寫完之後看到分數知道有錯，但是也沒有太多時間去看自己錯哪，畢竟作業交出去也隔一段時間了，一開始囫圇吞棗的寫，沒有真的去了解自己在寫什麼，後面要再看就直接放棄了。其實講這麼多只是想表達感謝老師和助教們很用心地改善了同學們寫作業的環境和方式，能打command就測試和交完作業真的很開心，之後有時間的話我看到TODO會試著幫忙寫一些的，再次感謝助教們和老師的用心，希望我這次能夠認真修完！
