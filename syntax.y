%{
	#define YYSTYPE struct TNode*	
	#include "lex.yy.c"
//	#include "Tree.h"
	#include <stdio.h>
//	int curLine;
	struct TNode* root;	
%}

/* declared tokens */
%token INT FLOAT ID
%token SEMI COMMA ASSIGNOP RELOP
%token PLUS MINUS STAR DIV AND OR
%token DOT NOT TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

/* associativity */
%right	ASSIGNOP
%left	OR
%left 	AND
%left	RELOP
%left	PLUS 	MINUS
%left	STAR 	DIV
%right	NOT
%left	LP RP LB RB DOT
%nonassoc	LOWER_THAN_ELSE
%nonassoc	ELSE

%%
/* declared High-Level Definitions */
Program		: ExtDefList			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Program"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);  /*if (errType == 0) printTree($$, 0);*/ root = $$}
		;
ExtDefList	: ExtDef ExtDefList		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDefList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); }
		| /* empty */			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Empty"; 
						$$->firstChild = NULL; $$->nextSibling = NULL;}
		;
ExtDef		: Specifier ExtDecList SEMI	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDef"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*global variable*/
		| Specifier SEMI		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDef"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*struct or int;*/
		| Specifier FunDec CompSt	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDef"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*function*/
		| error SEMI			/*error*/
		;
ExtDecList	: VarDec			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDecList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*ID*/
		| VarDec COMMA ExtDecList	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ExtDecList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*IDs*/
		;

/* declared Specifiers */
Specifier	: TYPE				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Specifier"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*int/float*/
		| StructSpecifier		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Specifier"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*struct*/
		;
StructSpecifier	: STRUCT OptTag LC DefList RC	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "StructSpecifier"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3); 
						insert($$, $4); insert($$, $5);}/*struct definition*/
		| STRUCT Tag			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "StructSpecifier"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}
		| STRUCT OptTag LC error RC	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "StructSpecifier"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3); 
						insert($$, $4); insert($$, $5);}
		;
OptTag		: ID				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "OptTag"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		| /* empty */			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "OptTag"; 
						$$->firstChild = NULL; $$->nextSibling = NULL;
						struct TNode* n = (struct TNode*)malloc(sizeof(struct TNode));
						n->name = "Empty"; 
						n->firstChild = NULL; n->nextSibling = NULL;
						insert($$, n);}
		;
Tag		: ID				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Tag"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		;

/* declared Declarators */
VarDec		: ID				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "VarDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}/*variable name*/
		| VarDec LB INT RB		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "VarDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}
		| VarDec LB error RB		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "VarDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}/* [][]? */  
		;
FunDec		: ID LP VarList RP		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "FunDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}/*function name*/
		| ID LP error RP		/*error*/
		| ID LP RP			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "FunDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| ID error RP			/*error*/
		;
VarList		: ParamDec COMMA VarList	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "VarList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*parameters*/
		| ParamDec			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "VarList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*empty?*/
		;
ParamDec	: Specifier VarDec		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "ParamDec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*parameter*/
		;

/* declared Statements */
CompSt		: LC DefList StmtList RC	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "CompSt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}/*c module*/
		;
StmtList	: Stmt StmtList			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "StmtList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*c statements*/
		| /* empty */			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "StmtList";
						$$->firstChild = NULL; $$->nextSibling = NULL;
						struct TNode* n = (struct TNode*)malloc(sizeof(struct TNode));
						n->name = "Empty"; 
						n->firstChild = NULL; n->nextSibling = NULL;
						insert($$, n);}
		;
Stmt		: Exp SEMI			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*expression*/
/*		| error SEMI			*/
		| CompSt			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*{...}*/
		| RETURN Exp SEMI		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*return...*/
		| IF LP Exp RP Stmt	%prec LOWER_THAN_ELSE	
						{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3); 
						insert($$, $4); insert($$, $5);}
		| IF LP Exp RP Stmt ELSE Stmt	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3); 
						insert($$, $4); insert($$, $5); insert($$, $6);
						insert($$, $7);}/*if... else...*/
		| IF LP error RP Stmt ELSE Stmt	/*error*/
		| WHILE LP Exp RP Stmt		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Stmt"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3); 
						insert($$, $4); insert($$, $5);}/*WHILE */
		;				/*for..?*/

/* declared Local Definitions */
DefList		: Def DefList			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "DefList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*regional variables*/
		| /* empty */			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "DefList"; 
						$$->firstChild = NULL; $$->nextSibling = NULL;
						struct TNode* n = (struct TNode*)malloc(sizeof(struct TNode));
						n->name = "Empty"; 
						n->firstChild = NULL; n->nextSibling = NULL;
						insert($$, n);}
		;
Def		: Specifier DecList SEMI	{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Def"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| error SEMI			
		;
DecList		: Dec				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "DecList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*variable names*/
		| Dec COMMA DecList		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "DecList"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		;
Dec		: VarDec			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Dec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}	/*variable name*/
		| VarDec ASSIGNOP Exp		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Dec"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*a = 5*/
		;

/* declared Expressions */
Exp		: Exp ASSIGNOP Exp		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*=*/
		| Exp AND Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*logical*/
		| Exp OR Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| Exp RELOP Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*compare*/
		| Exp PLUS Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*algorithm*/
		| Exp MINUS Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| Exp STAR Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| Exp DIV Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| LP Exp RP			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*()*/
		| MINUS Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*-...*/
		| NOT Exp			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2);}/*~*/
		| ID LP Args RP			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}/*call function*/
		| ID LP RP			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}
		| Exp LB Exp RB			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); 
						insert($$, $3); insert($$, $4);}/*visit array*/
		| Exp DOT ID			{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*visit struct*/
		| ID				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		| INT				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		| FLOAT				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Exp"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		| Exp LB error RB		
		;
Args		: Exp COMMA Args		{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Args"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1); insert($$, $2); insert($$, $3);}/*function parameter*/
		| Exp				{$$ = (struct TNode*)malloc(sizeof(struct TNode));
						$$->name = "Args"; $$->line = $1->line;
						$$->firstChild = NULL; $$->nextSibling = NULL;
						//printf("%s\n", $$->name);
						insert($$, $1);}
		;

%%
yyerror(char *msg) {
	errType = 2;
	printf("Error type 2 at line %d: %s\n", yylineno, msg);
}
