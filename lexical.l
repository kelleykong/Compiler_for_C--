%{
	#include"syntax.tab.h"
	#include "Tree.h"
	#include <stdio.h>
	#include <string.h>
	/*definitions of manifest constants
	INT, FLOAT, ID, 
	SEMI, COMMA, ASSIGNOP, RELOP,
	PLUS, MINUS, STAR, DIV, AND, OR,
	DOT, NOT, TYPE,
	LP, RP, LB, RB, LC, RC, 
	STRUCT, RETURN, IF, ELSE, WHILE 
	LT, LE, EQ, NE, GT, GE */
	int errType;
	
	float getFloat(char* str);
%}

delim	[ \t\n]
ws	{delim}+
letter	[A-Za-z]
digit	[0-9]
id	(_|{letter})({letter}|{digit}|_)*
%option yylineno

%%
struct		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "STRUCT";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return STRUCT;}
return		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RETURN";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RETURN;}
if		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "IF";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return IF;}
else		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "ELSE";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return ELSE;}
while		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "WHILE";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return WHILE;}
int		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "TYPE";
		n->tpe = string;
		n->str = "int";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return TYPE;}
float		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "TYPE";
		n->tpe = string;
		n->str = "float";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return TYPE;}
{ws}		{/*no action and no return */}
({digit}+)|(0(x|X)({digit}|[A-F]|[a-f])*) 	{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
//					char a[20] = "INT: ";		
//					n->name = malloc(20);
//					strcat(a, yytext);
//					n->name = strcpy(n->name, a);
					n->name = "INT";
					n->tpe = integer;
					n->x = getInt(yytext);
					n->line = yylineno;
					n->firstChild = NULL;
					n->nextSibling = NULL;
					yylval = n;
					 return INT;}
[-+]?({digit}*\.{digit}+|{digit}+\.)((e|E)[-+]?{digit}+)?	{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
//					char a[30] = "FLOAT: ";		
//					n->name = malloc(30);
//					strcat(a, yytext);
//					n->name = strcpy(n->name, a);
					n->name = "FLOAT";
					n->tpe = floatt;
					n->f = getFloat(yytext);
					n->line = yylineno;
					n->firstChild = NULL;
					n->nextSibling = NULL;
					yylval = n;
					 return FLOAT;}
{id}		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));		
		n->name = "ID";
		n->tpe = string;
		n->str = malloc(30);
		strcpy(n->str, yytext);
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
//		printf("%s\n", n->name);
		return ID;}
";"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "SEMI";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return SEMI;}
","		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "COMMA";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return COMMA;}
"="		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "ASSIGNOP";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return ASSIGNOP;}
"<"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = less;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RELOP;}
"<="		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = lessEqual;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RELOP;}
">"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = greater;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RELOP;}
">="		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = greaterEqual;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RELOP;}
"=="		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = equal;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;	
		 return RELOP;}
"!="		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RELOP";
		n->line = yylineno;
		n->tpe = relopp;
		n->relop = notEqual;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RELOP;}
"+"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "PLUS";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return PLUS;}
"-"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "MINUS";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;	
		 return MINUS;}
"*"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "STAR";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;		
		 return STAR;}
"/"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "DIV";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return DIV;}
"&&"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "AND";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return AND;}
"||"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "OR";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return OR;}
"."		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "DOT";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return DOT;}
"!"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "NOT";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return NOT;}
"("		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "LP";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return LP;}
")"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RP";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RP;}
"["		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "LB";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return LB;}
"]"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RB";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RB;}
"{"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "LC";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return LC;}
"}"		{struct TNode *n = (struct TNode*)malloc(sizeof(struct TNode));
		n->name = "RC";
		n->line = yylineno;
		n->firstChild = NULL;
		n->nextSibling = NULL;
		yylval = n;
		 return RC;}
"//"		{char c = input();
		while(c != '\n' && c != EOF) c = input();}
"/*"		{char c = input();
		while(c != EOF) {
			if (c == '*') {
				c = input();
				if (c == '/') break;
			}
			else if (c == '/') {
				c = input();
				if (c == '*')
					printf("Error type 3 at line %d: /* again\n",yylineno);
			}
			c = input();
		}
		if (c == EOF) 
			printf("Error type 3 at line %d: lack */\n", yylineno); }
"*/"		{}
.		{errType = 1; printf("Error type 1 at line %d:Mysterious character \'%s\'\n", yylineno, yytext);}

%%
int getInt(char *str) {
	if(str[0] == '0' && (str[1] == 'x' || str[1] =='X'))
		return HtoD(str);
	else if (str[0] == '0' && !(str[1] == 'x' || str[1] == 'X'))
		return OtoD(str);
	else
		return atoi(str);
}

int OtoD(char *str) {
	int sum = 0;
	int i;
	int num = strlen(str);
	for(i = 1; i < num; i++) {
		if (str[i] > '7')	/*D*/
			return atoi(str);
		sum = (sum + str[i] - '0') * 8;
	}
	sum = sum/8;
	return sum;
}

int HtoD(char *str) {
	int sum = 0;
	int i;
	int num = strlen(str);
	for(i = 2; i < num; i++) {
		if (str[i] >= '0' && str[i] <= '9')
			sum = (sum + str[i] - '0') * 16;
		else if (str[i] >= 'A' && str[i] <= 'F')
			sum = (sum + 10 + str[i] - 'A') *16;
	}
	sum = sum/16;
	return sum;
}

float aa(char *str, int n) {
	char *a = malloc(n+1);
	a = strncpy(a, str, n);
	char *b = malloc(strlen(str) - n);
	b = strncpy(b, str+n+1,strlen(str)-n-1);
	float f = atof(a);
	int i;
	if (atoi(b) >= 0) {
		for(i = 0; i < atoi(b); i++)
			f = f * 10;
	}
	else {
		for(i = 0; i > atoi(b); i--)
			f = f / 10;
	}
	return f;
}

float getFloat(char *str) {
	int num = strlen(str);
	int i;
	for(i = 0; i < num; i++) {
		if (str[i] == 'e' || str[i] == 'E') {
			float f = aa(str, i);
			return f;
		}
	}
	return atof(str);
}


