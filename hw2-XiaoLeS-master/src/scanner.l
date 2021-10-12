%{

#include <stdlib.h>

#include "parser.h"
#include "error.h"

#define LIST               strcat(buf, yytext)
#define token(t)           {LIST; if (Opt_T) printf("<%s>\n", #t);}
#define tokenChar(t)       {LIST; if (Opt_T) printf("<%c>\n", (t));}
#define tokenInteger(t, i) {LIST; if (Opt_T) printf("<%s: %d>\n", #t, (i));}
#define tokenOctInteger(t, i) {LIST; if (Opt_T) printf("<%s: %s>\n", #t, (i));}
#define tokenFloat(t, i) {LIST; if (Opt_T) printf("<%s: %s>\n", (t), (i));}
#define tokenScientific(t, i) {LIST; if (Opt_T) printf("<%s: %s>\n", (t), (i));}
#define tokenString(t, s)  {LIST; if (Opt_T) printf("<%s: %s>\n", #t, (s));}
// Define your token macros here
//  for example :
//    #define tokenScientific(t, s)   <definition>
//    #define tokenFloat(ts)          <definition>
//    #define <any other macros you needed>

#define MAX_LINE_LENG      256

int Opt_S = 1;
int Opt_T = 1;
int linenum = 1;
char buf[MAX_LINE_LENG];
char strBuf[MAX_LINE_LENG]; // String Buffer

%}

whitespace	[ \t]+
identifier	[a-zA-Z][a-zA-Z0-9]*
integer		0|[1-9][0-9]*
octal		0[0-7]+
float		{integer}"."(0|[0-9]*[1-9])
scientific	({integer}|{float})[Ee][+-]?({integer})
string		\"((\"\")|[^\"\n])*\"

/* For Comments */
%x COMMENT_C
%x COMMENT_CPP

%%
","		{ tokenChar(','); return M_COMMA; }
";"		{ tokenChar(';'); return M_SEMICOLON; }
":"		{ tokenChar(':'); return M_COLON; }
"("     { tokenChar('('); return M_L_PAR; }
")"		{ tokenChar(')'); return M_R_PAR; }
"["		{ tokenChar('['); return M_L_SQBRA; }
"]"		{ tokenChar(']'); return M_R_SQBRA; }

"+"		{ tokenChar('+'); return OP_ADD; }
"-"		{ tokenChar('-'); return OP_SUB; }
"*"		{ tokenChar('*'); return OP_MUL; }
"/"		{ tokenChar('/'); return OP_DIV; }
"mod"	{ token(mod); return OP_MOD; }
":="	{ token(:=); return OP_ASSIGN; }
"<"		{ tokenChar('<'); return OP_LT; }
"<="	{ token(<=); return OP_LE; }
"<>"	{ token(<>); return OP_NE; }
">="	{ token(>=); return OP_GE; }
">"		{ tokenChar('>'); return OP_GT; }
"="		{ tokenChar('='); return OP_EQ; }
"and"	{ token(and); return OP_AND; }
"or"	{ token(or); return OP_OR; }
"not"	{ token(not); return OP_NOT; }


"array"		{ token(KWarray); return ARRAY; }
"begin" 	{ token(KWbegin); return BEG; }
"boolean" 	{ token(KWboolean); return BOOL; }
"def"		{ token(KWdef); return DEF; }
"do"		{ token(KWdo); return DO; }
"else"		{ token(KWelse); return ELSE; }
"end"		{ token(KWend); return END; }
"false"		{ token(KWfalse); return FALSE; }
"for"		{ token(KWfor); return FOR; }
"integer"	{ token(KWinteger); return INTEGER; }
"if"		{ token(KWif); return IF; }
"of"		{ token(KWof); return OF; }
"print"		{ token(KWprint); return PRINT; }
"read"		{ token(KWread); return READ; }
"real"		{ token(KWreal); return REAL; }
"string"	{ token(KWstring); return STRING; }
"then"		{ token(KWthen); return THEN; }
"to"		{ token(KWto); return TO; }
"true"		{ token(KWtrue); return TRUE; }
"return"	{ token(KWreturn); return RETURN; }
"var"		{ token(KWvar); return VAR; }
"while"		{ token(KWwhile); return WHILE; }

{integer}	{ tokenInteger(integer, atoi(yytext)); return CONST_INT; }
{octal}		{ tokenOctInteger(oct_integer, yytext); return CONST_OCT; }
{float}		{ tokenFloat("float", yytext); return CONST_FLOAT; }
{scientific} { tokenScientific("scientific", yytext); return CONST_SCI; }
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

				return CONST_STRING;
			}

{identifier} { tokenString(id, yytext); return ID; }

"//&"[A-Z][+-][^\n]*	{
							LIST;
							switch(yytext[3]) {
								case 'S':
									if (yytext[4] == '+')
										Opt_S = 1;
									else
										Opt_S = 0;
									break;
								case 'T':
									if (yytext[4] == '+')
										Opt_T = 1;
									else
										Opt_T = 0;
									break;
								default:
									break;
							}
						}

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

"/*"	{ LIST; BEGIN COMMENT_C; }
<COMMENT_C>"\n"		{
						LIST;
						if (Opt_S)
							printf("%d: %s", linenum, buf);
						linenum++;
						buf[0] = '\0';
					}

<COMMENT_C>"*/"		{ LIST; BEGIN INITIAL; }
<COMMENT_C>.		{ LIST; }

{whitespace} { LIST; }

\n      {
          LIST;
          if (Opt_S)
            printf("%d: %s", linenum, buf);
          linenum++;
          buf[0] = '\0';
        }
.       {
          /* error */
          printf("Error at line %d: bad character \"%s\"\n", linenum, yytext );
          exit(-1);
        }
%%
