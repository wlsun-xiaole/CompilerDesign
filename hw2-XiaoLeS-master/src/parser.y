%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "error.h"

extern int32_t linenum;   /* declared in scanner.l */
extern char buf[512];  /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token M_COMMA
%token M_SEMICOLON
%token M_COLON
%token M_L_PAR
%token M_R_PAR
%token M_L_SQBRA
%token M_R_SQBRA

%token OP_ADD
%token OP_SUB
%token OP_MUL
%token OP_DIV
%token OP_MOD
%token OP_ASSIGN
%token OP_LT
%token OP_LE
%token OP_NE
%token OP_GE
%token OP_GT
%token OP_EQ
%token OP_AND
%token OP_OR
%token OP_NOT

%left OP_ADD OP_SUB OP_MUL OP_DIV OP_MOD OP_AND OP_OR OP_NOT

%token ARRAY
%token BEG
%token BOOL
%token DEF
%token DO
%token ELSE
%token END
%token FALSE
%token FOR
%token INTEGER
%token IF
%token OF
%token PRINT
%token READ
%token REAL
%token STRING
%token THEN
%token TO
%token TRUE
%token RETURN
%token VAR
%token WHILE

%token CONST_INT
%token CONST_OCT
%token CONST_FLOAT
%token CONST_SCI
%token CONST_STRING

%token ID

%start program
%%

program	:	ID M_SEMICOLON
			list_var_const_decl
			list_func_decl
			a_stat_compound 
			END
		;

list_var_const_decl	:
					|	list_var_const_decl1
					;
list_var_const_decl1:	a_var_const_decl
					|	list_var_const_decl1
						a_var_const_decl
					;
a_var_const_decl	:	VAR list_id M_COLON type_scalar	M_SEMICOLON
					|	VAR list_id M_COLON type_array M_SEMICOLON
					|	VAR list_id M_COLON const_literal M_SEMICOLON
					;

list_id				:	ID
					|	list_id M_COMMA ID	
					;
type_scalar	:	INTEGER
			|	REAL
			|	STRING
			|	BOOL
			;

type_array	:	ARRAY const_integer	OF type
			;
const_integer	:	CONST_INT
				|	CONST_OCT
				;

type	:	type_scalar
		|	type_array
		;

const_literal	:	const_integer
				|	OP_SUB const_integer
				|	CONST_FLOAT
				|	OP_SUB CONST_FLOAT
				|	CONST_SCI
				|	OP_SUB CONST_SCI
				|	CONST_STRING
				|	TRUE
				|	FALSE
				;

const_literal_no_neg	:	const_integer
						|	CONST_FLOAT
						|	CONST_SCI
						|	CONST_STRING
						|	TRUE
						|	FALSE
						;

list_func_decl	:	
				| 	list_func_decl1
				;
list_func_decl1 :	a_func_decl
				|	list_func_decl1
					a_func_decl
				;
a_func_decl		:	ID M_L_PAR list_arg M_R_PAR type_return M_SEMICOLON
				|	ID M_L_PAR list_arg M_R_PAR type_return 
					a_stat_compound
					END
				;
type_return	:
			|	M_COLON type_scalar
			;

list_arg	:	
			|	list_arg1
			;
list_arg1	:	a_arg
			| 	list_arg1 M_SEMICOLON a_arg
			;
a_arg		:	list_id M_COLON type
			;

list_stat	:
			|	list_stat1
			;
list_stat1	:	a_stat
			|	list_stat1
				a_stat
			;
a_stat		:	a_stat_compound
			|	a_stat_simple
			|	a_stat_conditional
			|	a_stat_while
			|	a_stat_for
			|	a_stat_return
			|	a_stat_proc_call
			;

a_stat_compound	:	BEG
					list_var_const_decl
					list_stat
					END
				;
a_stat_simple	:	var_ref OP_ASSIGN a_expression M_SEMICOLON
				|	PRINT var_ref M_SEMICOLON
				|	PRINT a_expression M_SEMICOLON
				|	READ var_ref M_SEMICOLON
				;
var_ref		:	ID
			|	var_ref array_ref
			;
array_ref	:	M_L_SQBRA a_expression M_R_SQBRA
			;

a_stat_conditional	:	IF a_expression THEN
						a_stat_compound
						ELSE
						a_stat_compound
						END IF
					|	IF a_expression THEN
						a_stat_compound
						END IF
					;

a_stat_while	:	WHILE a_expression DO
					a_stat_compound
					END DO
				;

a_stat_for	:	FOR ID OP_ASSIGN const_integer	TO const_integer DO
				a_stat_compound
				END DO
				;

a_stat_return	:	RETURN a_expression M_SEMICOLON
				;

a_stat_proc_call	:	ID M_L_PAR list_expression M_R_PAR M_SEMICOLON
					;

list_expression	:
				|	list_expression1
				;
list_expression1:	a_expression
				|	list_expression1 M_COMMA a_expression
				;
a_expression	:	const_literal
				|	var_ref
				|	func_invocation
				|	M_L_PAR a_expression M_R_PAR
				|	math_binary_substraction
				|	relational_binary_substraction
				|	unary_negation
				;
func_invocation	:	ID M_L_PAR list_expression M_R_PAR
				;

math_binary_substraction	:	a_expression math_binary_ops a_expression_no_unary
					;

relational_binary_substraction	:	a_expression rel_binary_ops a_expression
							;
unary_negation	:	unary_ops a_expression_no_unary
				;
a_expression_no_unary	:	const_literal_no_neg
						|	var_ref
						|	func_invocation
						|	M_L_PAR a_expression M_R_PAR
						;

math_binary_ops	:	OP_ADD
			|	OP_SUB
			|	OP_MUL
			|	OP_DIV
			|	OP_MOD

rel_binary_ops:	OP_LT
			|	OP_LE
			|	OP_NE
			|	OP_GE
			|	OP_GT
			|	OP_EQ
			|	OP_AND
			|	OP_OR
			;

unary_ops	:	OP_SUB
			|	OP_NOT
			;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            linenum, buf, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    CHECK(argc == 2, "Usage: ./parser <filename>\n");

    FILE *fp = fopen(argv[1], "r");

    CHECK(fp != NULL, "fopen() fails.\n");
    yyin = fp;
    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
