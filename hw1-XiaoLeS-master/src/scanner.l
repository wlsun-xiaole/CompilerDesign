%{

#include <stdlib.h>

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
","		{ tokenChar(','); }
";"		{ tokenChar(';'); }
":"		{ tokenChar(':'); }
"("     { tokenChar('('); }
")"		{ tokenChar(')'); }
"["		{ tokenChar('['); }
"]"		{ tokenChar(']'); }

"+"		{ tokenChar('+'); }
"-"		{ tokenChar('-'); }
"*"		{ tokenChar('*'); }
"/"		{ tokenChar('/'); }
"mod"	{ token(mod); }
":="	{ token(:=); }
"<"		{ tokenChar('<'); }
"<="	{ token(<=); }
"<>"	{ token(<>); }
">="	{ token(>=); }
">"		{ tokenChar('>'); }
"="		{ tokenChar('='); }
"and"	{ token(and); }
"or"	{ token(or); }
"not"	{ token(not); }


"array"		{ token(KWarray); }
"begin" 	{ token(KWbegin); }
"boolean" 	{ token(KWboolean); }
"def"		{ token(KWdef); }
"do"		{ token(KWdo); }
"else"		{ token(KWelse); }
"end"		{ token(KWend); }
"false"		{ token(KWfalse); }
"for"		{ token(KWfor); }
"integer"	{ token(KWinteger); }
"if"		{ token(KWif); }
"of"		{ token(KWof); }
"print"		{ token(KWprint); }
"read"		{ token(KWread); }
"real"		{ token(KWreal); }
"string"	{ token(KWstring); }
"then"		{ token(KWthen); }
"to"		{ token(KWto); }
"true"		{ token(KWtrue); }
"return"	{ token(KWreturn); }
"var"		{ token(KWvar); }
"while"		{ token(KWwhile); }

{integer}	{ tokenInteger(integer, atoi(yytext)); }
{octal}		{ tokenOctInteger(oct_integer, yytext); }
{float}		{ tokenFloat("float", yytext); }
{scientific} { tokenScientific("scientific", yytext); }
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

{identifier} { tokenString(id, yytext); }

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
int main( int argc, char **argv )
{
  if( argc != 2 ) {
    fprintf( stderr, "Usage: ./scanner [filename]\n" );
    exit(0);
  }
	
  FILE *fp = fopen( argv[1], "r" );
  if( fp == NULL ) {
    fprintf( stderr, "Open file error\n" );
    exit(-1);
  }
	
  yyin = fp;

  yylex();

  exit(0);
}
