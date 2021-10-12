%{
#include <vector>
#include <utility>
#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>


#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

/* Declared by scanner.l */
extern uint32_t line_num;
extern char buffer[512];
extern FILE *yyin;
extern char *yytext;
/* End */

static ProgramNode *root;
static int decl_cnt = 0;
extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

%}

%code requires {
    #include <utility>
    #include <vector>
    #include "AST/ast.hpp"
    #include "AST/decl.hpp"
    #include "AST/function.hpp"
    #include "AST/CompoundStatement.hpp"
    #include "AST/expression.hpp"
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    int integer;
    VarType var_type;
    double real;
    bool boolean;
    
    /* Declarations */
    DeclNode *decl_node;
    std::vector<DeclNode> *list_decl_node;
    std::vector<IdPack> *list_p_id_pack;
    std::vector<int> *list_int;
    struct VarTypePack *pack_var_type;
    struct ConstValPack* const_value_pack;
    union ConstVal *const_val;

    /* Functions */
    FunctionNode *func_node;
    std::vector<FunctionNode> *list_func_node;

    /* Compound Stats */
    CompoundStatementNode *comp_stat_node;
    std::vector<AstNode*> *list_p_ast_node;
    AstNode *ast_node;

    /* Expressions */
    ExpressionNode *exp_node;
    std::vector<ExpressionNode*>* list_p_exp_node;

    /* Variable References */
    VariableReferenceNode *var_ref_node;

};


%type <identifier> ProgramName ID STRING_LITERAL FunctionName
%type <list_decl_node> DeclarationList Declarations FormalArgList FormalArgs
%type <decl_node> Declaration FormalArg
%type <pack_var_type> Type ArrType ReturnType
%type <integer> INT_LITERAL NegOrNot 
%type <var_type> ScalarType
%type <list_p_id_pack> IdList
%type <list_int> ArrDecl
%type <const_value_pack> LiteralConstant StringAndBoolean IntegerAndReal
%type <real> REAL_LITERAL
%type <boolean> TRUE FALSE
%type <func_node> Function FunctionDeclaration FunctionDefinition
%type <list_func_node> FunctionList Functions
%type <comp_stat_node> CompoundStatement ElseOrNot
%type <list_p_ast_node> StatementList Statements
%type <ast_node> Statement Simple FunctionCall Condition While For Return
%type <exp_node> Expression FunctionInvocation
%type <list_p_exp_node> Expressions ExpressionList ArrRefs ArrRefList
%type <var_ref_node> VariableReference

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%
    /*
       Program Units
                     */

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

ProgramName:
    ID
;

DeclarationList:
    Epsilon { $$ = new std::vector<DeclNode>; }
    |
    Declarations { $$ = $1; }
;

Declarations:
    Declaration {
        DeclNode *p = $1;
        $$ = new std::vector<DeclNode>;
        $$->push_back(*p);
    }
    |
    Declarations Declaration {
        DeclNode *p = $2;
        $$->push_back(*p);
    }
;

FunctionList:
    Epsilon { $$ = new std::vector<FunctionNode>; }
    |
    Functions { $$ = $1; }
;

Functions:
    Function {
        FunctionNode *p = $1;
        $$ = new std::vector<FunctionNode>;
        $$->push_back(*p);
    }
    |
    Functions Function {
        FunctionNode *p = $2;
        $$->push_back(*p);
    }
;

Function:
    FunctionDeclaration { $$ = $1; }
    |
    FunctionDefinition { $$ = $1; }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, NULL);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6);
    }
;

FunctionName:
    ID
;

FormalArgList:
    Epsilon { $$ = new std::vector<DeclNode>; }
    |
    FormalArgs { $$ = $1; }
;

FormalArgs:
    FormalArg {
        $$ = new std::vector<DeclNode>;
        DeclNode *p = $1;
        $$->push_back(*p);
    }
    |
    FormalArgs SEMICOLON FormalArg {
        DeclNode *p = $3;
        $$->push_back(*p);
    }
;

FormalArg:
    IdList COLON Type {
        $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3);
    }
;

IdList:
    ID {
        struct IdPack *id_pack = new struct IdPack;
        id_pack->name = $1;
        id_pack->location.first = @1.first_line;
        id_pack->location.second = @1.first_column;
        $$ = new std::vector<IdPack>;
        $$->push_back(*id_pack);
    }
    |
    IdList COMMA ID {
        struct IdPack *id_pack = new struct IdPack;
        id_pack->name = $3;
        id_pack->location.first = @3.first_line;
        id_pack->location.second = @3.first_column;
        $$->push_back(*id_pack);
    }
;

ReturnType:
    COLON ScalarType {
        $$ = new struct VarTypePack;
        $$->type = $2;
        $$->dims = NULL;
    }
    |
    Epsilon { 
        $$ = new struct VarTypePack;
        $$->type = VarType_VOID;
        $$->dims = NULL;
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        // create decl node
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        // create decl node
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
;

Type:
    ScalarType {
        $$ = new struct VarTypePack;
        $$->type = $1;
        $$->dims = NULL;
    }
    |
    ArrType {
        $$ = $1;
    }
;

ScalarType:
    INTEGER {
        $$ = VarType_INTEGER;
    }
    |
    REAL {
        $$ = VarType_REAL;
    }
    |
    STRING {
        $$ = VarType_STRING;
    }
    |
    BOOLEAN {
        $$ = VarType_BOOLEAN;
    }
;

ArrType:
    ArrDecl ScalarType {
        $$ = new struct VarTypePack;
        $$->type = $2;
        $$->dims = $1;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        $$ = new std::vector<int>;
        $$->push_back($2);
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        $$->push_back($3);
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        $$ = new struct ConstValPack;
        $$->type = VarType_INTEGER;
        $$->const_val.ConstVal_INTEGER = $1 * $2;
        $$->location.first = @1.first_line ? @1.first_line : @2.first_line;
        $$->location.second = @1.first_column ? @1.first_column : @2.first_column;
    }
    |
    NegOrNot REAL_LITERAL {
        $$ = new struct ConstValPack;
        $$->type = VarType_REAL;
        $$->const_val.ConstVal_REAL = $1 * $2;
        $$->location.first = @1.first_line ? @1.first_line : @2.first_line;
        $$->location.second = @1.first_column ? @1.first_column : @2.first_column;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon { $$ = 1; }
    |
    MINUS %prec UNARY_MINUS { $$ = -1; }
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = new struct ConstValPack;
        $$->type = VarType_STRING;
        $$->const_val.ConstVal_STRING = $1;
        $$->location.first = @1.first_line;
        $$->location.second = @1.first_column;
    }
    |
    TRUE {
        $$ = new struct ConstValPack;
        $$->type = VarType_BOOLEAN;
        $$->const_val.ConstVal_BOOLEAN = $1;
        $$->location.first = @1.first_line;
        $$->location.second = @1.first_column;
    }
    |
    FALSE {
        $$ = new struct ConstValPack;
        $$->type = VarType_BOOLEAN;
        $$->const_val.ConstVal_BOOLEAN = $1;
        $$->location.first = @1.first_line;
        $$->location.second = @1.first_column;
    }
;

IntegerAndReal:
    INT_LITERAL {
        $$ = new struct ConstValPack;
        $$->type = VarType_INTEGER;
        $$->const_val.ConstVal_INTEGER = $1;
        $$->location.first = @1.first_line;
        $$->location.second = @1.first_column;
    }
    |
    REAL_LITERAL {
        $$ = new struct ConstValPack;
        $$->type = VarType_REAL;
        $$->const_val.ConstVal_REAL = $1;
        $$->location.first = @1.first_line;
        $$->location.second = @1.first_column;
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = $1;
    }
    |
    Simple { $$ = $1; }
    |
    Condition { $$ = $1; }
    |
    While { $$ = $1; }
    |
    For { $$ = $1; }
    |
    Return { $$ = $1; }
    |
    FunctionCall {
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {
       $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        std::vector<ExpressionNode*>* p = new std::vector<ExpressionNode*>;
        p->push_back($3);
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, p);
    }
    |
    PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
    }
;

ArrRefList:
    Epsilon { $$ = new std::vector<ExpressionNode*>; }
    |
    ArrRefs { $$ = $1; }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = new std::vector<ExpressionNode*>;
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $$->push_back($3);
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        std::vector<ExpressionNode*> * ep = new std::vector<ExpressionNode*>;
        ep->push_back($2);
        std::vector<CompoundStatementNode> *p 
            = new std::vector<CompoundStatementNode>;
        p->push_back(*$4);
        if ($5 != NULL) p->push_back(*$5);
        $$ = new IfNode(@1.first_line, @1.first_column, ep, p);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($2);
        $$ = new WhileNode(@1.first_line, @1.first_column, p, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        // Declaration
            // IdList
        struct IdPack *id_pack = new struct IdPack;
        id_pack->name = $2;
        id_pack->location.first = @2.first_line;
        id_pack->location.second = @2.first_column;
        std::vector<IdPack>* list_id = new std::vector<IdPack>;
        list_id->push_back(*id_pack);
            // Type
        struct VarTypePack *vp = new struct VarTypePack;
        vp->type = VarType_INTEGER;
        vp->dims = NULL;

        DeclNode* decl = new DeclNode(@2.first_line, @2.first_column, list_id, vp);

        // Assignment
            // VarRef
        std::vector<ExpressionNode*> *empty_exp_vector
            = new std::vector<ExpressionNode*>;
        VariableReferenceNode* vrp = new VariableReferenceNode(@2.first_line,
            @2.first_column, $2, empty_exp_vector);
            // first ConstVal & Exp
        struct ConstValPack* cvpp1 = new struct ConstValPack;
        cvpp1->type = VarType_INTEGER;
        cvpp1->const_val.ConstVal_INTEGER = $4;
        cvpp1->location.first = @4.first_line;
        cvpp1->location.second = @4.first_column;

        ExpressionNode* cp1 = new ConstantValueNode(cvpp1);
        std::vector<ExpressionNode*>* list_ep = new std::vector<ExpressionNode*>;
        list_ep->push_back(cp1);

        AssignmentNode* assig 
            = new AssignmentNode(@3.first_line, @3.first_column, vrp, list_ep);

        // second ConstVal
        struct ConstValPack* cvpp2 = new struct ConstValPack;
        cvpp2->type = VarType_INTEGER;
        cvpp2->const_val.ConstVal_INTEGER = $6;
        cvpp2->location.first = @6.first_line;
        cvpp2->location.second = @6.first_column;
        ConstantValueNode* cp2 = new ConstantValueNode(cvpp2);

        // For
        $$ = new ForNode(@1.first_line, @1.first_column, decl, assig, cp2, $8);
    }
;

Return:
    RETURN Expression SEMICOLON {
        std::vector<ExpressionNode*> *exps = 
            new std::vector<ExpressionNode*>;
        exps->push_back($2); 
        $$ = new ReturnNode(@1.first_line, @1.first_column, exps);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON {
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon { $$ = new std::vector<ExpressionNode*>; }
    |
    Expressions { $$ = $1; }
;

Expressions:
    Expression {
        $$ = new std::vector<ExpressionNode*>;
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression {
        $$->push_back($3);
    }
;

StatementList:
    Epsilon { $$ = new std::vector<AstNode*>; }
    |
    Statements { $$ = $1; }
;

Statements:
    Statement {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Statements Statement {
        $$->push_back($2);
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($2);
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column,
                                        OpType_MINUS, p);
    }
    |
    Expression MULTIPLY Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_MULTIPLY, p);
    }
    |
    Expression DIVIDE Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_DIVIDE, p);
    }
    |
    Expression MOD Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_MOD, p);
    }
    |
    Expression PLUS Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_PLUS, p);
    }
    |
    Expression MINUS Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_MINUS, p);
    }
    |
    Expression LESS Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_LESS, p);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_LESS_OR_EQUAL, p);
    }
    |
    Expression GREATER Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_GREATER, p);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_GREATER_OR_EQUAL, p);
    }
    |
    Expression EQUAL Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_EQUAL, p);
    }
    |
    Expression NOT_EQUAL Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_NOT_EQUAL, p);
    }
    |
    NOT Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($2);
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column,
                                        OpType_NOT, p);
    }
    |
    Expression AND Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_AND, p);
    }
    |
    Expression OR Expression {
        std::vector<ExpressionNode*> *p = new std::vector<ExpressionNode*>;
        p->push_back($1);
        p->push_back($3);
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column,
                                        OpType_OR, p);
    }
    |
    IntegerAndReal {
        $$ = new ConstantValueNode($1);
    }
    |
    StringAndBoolean {
        $$ = new ConstantValueNode($1);
    }
    |
    VariableReference { $$ = $1; }
    |
    FunctionInvocation { $$ = $1; }
;

    /*
       misc
            */
Epsilon:
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
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    assert(argc >= 2 && "Usage: ./parser <filename> [--dump-ast]\n");

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.\n");

    yyparse();

    if (argc > 2 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper dumper;
        dumper.visit(*root);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
