#include "ICGen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IDList.h"
//#include "Tree.h"

int labelNo = 0;
int varNo = 0;
int tempVNo = 0;

struct ICNode* IChead;

void printIC(struct InterCode *code);

void translateExtDef(struct TNode *ExtDef, struct HashEntry ht[]);
void translateExtDecList(struct TNode *ExtDecList, struct HashEntry ht[]);
struct Operand* translateVarDec(struct TNode *VarDec, struct HashEntry ht[], struct Operand* place);
void translateFunDec(struct TNode *FunDec, struct HashEntry ht[]);
void translateVarList(struct TNode *VarList, struct HashEntry ht[]);
void translateParamDec(struct TNode *ParamDec, struct HashEntry ht[]);
struct ICNode* translateCompSt(struct TNode *CompSt, struct HashEntry ht[]);
struct ICNode* translateStmtList(struct TNode *StmtList, struct HashEntry ht[]);
struct ICNode* translateStmt(struct TNode *Stmt, struct HashEntry ht[]);
struct ICNode* translateDefList(struct TNode *DefList, struct HashEntry ht[]);
struct ICNode* translateDef(struct TNode *Def, struct HashEntry ht[]);
struct ICNode* translateDecList(struct TNode *DecList, struct HashEntry ht[]);
struct ICNode* translateDec(struct TNode *Dec, struct HashEntry ht[]);
struct ICNode* translateExp(struct TNode *Exp, struct HashEntry ht[], struct Operand *place);
struct ICNode* translateArgs(struct TNode *Args, struct HashEntry ht[], struct Operand *argList);
struct ICNode* translateCond(struct TNode *Exp, struct ICNode *labelTrue, struct ICNode *labelFalse, struct HashEntry ht[]);


struct Operand* OpVarInit()
{
	struct Operand *op = (struct Operand*)malloc(sizeof(struct Operand));
	op->kind = VARIABLE;
	op->u.varNo = ++varNo;
}

struct Operand* OpAddrInit(int add)
{
	struct Operand *op = (struct Operand*)malloc(sizeof(struct Operand));
	op->kind = ADDRESS;
	op->u.addr = add;
}

struct Operand* OpTmpInit()
{
	struct Operand *op = (struct Operand*)malloc(sizeof(struct Operand));
	op->kind = TEMPV;
	op->u.tmpNo = ++tempVNo;
}

struct Operand* OpConstInit(int v)
{
	struct Operand *op = (struct Operand*)malloc(sizeof(struct Operand));
	op->kind = CONSTANT;
	op->u.value = v;
}

struct Operand* OpArryInit()
{
	struct Operand *op = (struct Operand*)malloc(sizeof(struct Operand));
	op->kind = ARRAY;
	op->u.array.varNo = ++varNo;
}

struct ICNode* ICNodeLabelInit(int k, int lNo)
{
//	printf("ICNode LABEL or BR INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	n->code->u.labelNo = lNo;
	return n;
}

struct ICNode* ICNodeFuncInit(int k, char* str, int sz)
{
//	printf("ICNode FUNC INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	if (k == FUNC) {
		n->code->u.func.funcName = str;
		n->code->u.func.paramNum = sz;
	}
	return n;
}

struct ICNode* ICNodeAssignInit(int k, struct Operand* op1, struct Operand* op2)
{
//	printf("ICNode Assign INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	if (k == ASSIGN) {
		n->code->u.assign.left = op1;
		n->code->u.assign.right = op2;
	}
	printIC(n->code);
	return n;
}

struct ICNode* ICNodeBinopInit(int k, struct Operand* r, struct Operand* op1, struct Operand* op2)
{
//	printf("ICNode op2 INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	n->code->u.binop.result = r;
	n->code->u.binop.op1 = op1;
	n->code->u.binop.op2 = op2;
	return n;
}	

struct ICNode* ICNodeSolopInit(int k, struct Operand* r, struct Operand* op)
{
//	printf("ICNode op1 INIT!\n");	
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	n->code->u.solop.result = r;
	n->code->u.solop.op = op;
	return n;
}

struct ICNode* ICNodeMemInit(int k, struct Operand* add, struct Operand* r)
{
//	printf("ICNode LOAD OR STORE INIT!\n");	
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	n->code->u.mem.addr = add;
	n->code->u.mem.result = r;
	return n;
}

struct ICNode* ICNodeBrelopInit(int k, struct Operand* op1, struct Operand* op2, int relop, int lNo)
{
//	printf("ICNode Brelop INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	if (k == Brelop) {
		n->code->u.brelop.x = op1;
		n->code->u.brelop.y = op2;
		n->code->u.brelop.relop = relop;
		n->code->u.brelop.labelNo = lNo;
	}
	return n;
}

struct ICNode* ICNodeDecInit(int k, struct Operand* a, int sz)
{
//	printf("ICNode DEC INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	if (k == DEC) {
		n->code->u.dec.array = a;
		n->code->u.dec.size = sz;
	}
	return n;
}

struct ICNode* ICNodeCallInit(int k, struct Operand* r, char* str)
{
//	printf("ICNode FUNC INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	if (k == CALL) {
		n->code->u.call.result = r;
		n->code->u.call.funcName = str;
	}
	return n;
}

struct ICNode* ICNodeRWInit(int k, struct Operand *op)
{
//	printf("ICNode READ OR WRITE OR RETRUN OR ARG PARAM INIT!\n");
	struct ICNode* n = (struct ICNode*)malloc(sizeof(struct ICNode));
	n->code = (struct InterCode*)malloc(sizeof(struct InterCode));
	n->code->kind = k;
	n->code->u.rw.op = op;
	return n;
}

struct ICNode* ICTableInsert(struct ICNode *p, struct ICNode *n)
{
//	printf("ICTable Insert!\n");
	if (p == NULL) {
		IChead = n;
		n->next = n;
		n->prev = n;
		return n;
	}
	struct ICNode *t = p->prev;
	t->next = n;
	n->next = p;
	p->prev = n;
	n->prev = t;
	return n;
}

void printOperand(struct Operand* op)
{
//	printf("printOperand\n");
	if (op->kind == VARIABLE)
		printf("v%d", op->u.varNo);
	else if (op->kind == ADDRESS)
		printf("&v%d", op->u.addr);
	else if (op->kind == TEMPV)
		printf("t%d", op->u.tmpNo);
	else if (op->kind == CONSTANT)
		printf("#%d", op->u.value);
	else if (op->kind == ARRAY)
		printf("v%d", op->u.array.varNo);
}	

void printOperandtoF(FILE *fp, struct Operand* op)
{
	if (op->kind == VARIABLE)
		fprintf(fp, "v%d", op->u.varNo);
	else if (op->kind == ADDRESS)
		fprintf(fp, "&v%d", op->u.addr);
	else if (op->kind == TEMPV)
		fprintf(fp, "t%d", op->u.varNo);
	else if (op->kind == CONSTANT)
		fprintf(fp, "#%d", op->u.value);
	else if (op->kind == ARRAY)
		fprintf(fp, "v%d", op->u.array.varNo);
}

void printIC(struct InterCode *code)
{
//	printf("printIC \n");
	if (code->kind == LABEL)
		printf("LABEL label%d :\n", code->u.labelNo);
	else if (code->kind == FUNC)
		printf("FUNCTION %s :\n", code->u.func.funcName);
	else if (code->kind == ASSIGN) {
		printOperand(code->u.assign.left);
		printf(" := ");
		printOperand(code->u.assign.right);
		printf("\n");
	}
	else if (code->kind == ADD) {
		printOperand(code->u.binop.result);
		printf(" := ");
		printOperand(code->u.binop.op1);
		printf(" + ");
		printOperand(code->u.binop.op2);
		printf("\n");
	}
	else if (code->kind == SUB) {
		printOperand(code->u.binop.result);
		printf(" := ");
		printOperand(code->u.binop.op1);
		printf(" - ");
		printOperand(code->u.binop.op2);
		printf("\n");
	}
	else if (code->kind == MUL) {
		printOperand(code->u.binop.result);
		printf(" := ");
		printOperand(code->u.binop.op1);
		printf(" * ");
		printOperand(code->u.binop.op2);
		printf("\n");
	}
	else if (code->kind == DIV) {
		printOperand(code->u.binop.result);
		printf(" := ");
		printOperand(code->u.binop.op1);
		printf(" / ");
		printOperand(code->u.binop.op2);
		printf("\n");
	}
	else if (code->kind == LOAD) {
		printOperand(code->u.mem.result);
		printf(" := *");
		printOperand(code->u.mem.addr);
		printf("\n");
	}
	else if (code->kind == STORE) {
		printf("*");
		printOperand(code->u.mem.addr);
		printf(" := ");
		printOperand(code->u.mem.result);
		printf("\n");
	}
	else if (code->kind == BR) 
		printf("GOTO label%d\n", code->u.labelNo);
	else if (code->kind == Brelop) {
		printf("IF ");
		printOperand(code->u.brelop.x);
		switch (code->u.brelop.relop) {
			case equal: printf(" == "); break;
			case notEqual: printf(" != "); break;
			case greater: printf(" > "); break;
			case greaterEqual: printf(" >= "); break;
			case less: printf(" < "); break;
			case lessEqual: printf(" <= "); break;
		}
		printOperand(code->u.brelop.y);
		printf("\t GOTO label%d\n", code->u.brelop.labelNo);
	}
	else if (code->kind == DEC) {
		printf("DEC ");
		printOperand(code->u.dec.array);
		printf(" %d\n", code->u.dec.size);
	}
	else if (code->kind == CALL) {
		printOperand(code->u.call.result);
		printf(" := CALL %s\n", code->u.call.funcName);
	}
	else if (code->kind == RETURN) {
		printf("RETURN ");
		printOperand(code->u.rw.op);
		printf("\n");
	}
	else if (code->kind == ARG) {
		printf("ARG ");
		printOperand(code->u.rw.op);
		printf("\n");
	}
	else if (code->kind == PARAM) {
		printf("PARAM ");
		printOperand(code->u.rw.op);
		printf("\n");
	}
	else if (code->kind == READ) {
		printf("READ ");
		printOperand(code->u.rw.op);
		printf("\n");
	}
	else if (code->kind == WRITE) {
		printf("WRITE ");
		printOperand(code->u.rw.op);
		printf("\n");
	}
}

void printICtoF(FILE *fp, struct InterCode *code)
{
	if (code->kind == LABEL)
		fprintf(fp, "LABEL label%d :\n", code->u.labelNo);
	else if (code->kind == FUNC)
		fprintf(fp, "FUNCTION %s :\n", code->u.func.funcName);
	else if (code->kind == ASSIGN) {
		printOperandtoF(fp, code->u.assign.left);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.assign.right);
		fprintf(fp, "\n");
	}
	else if (code->kind == ADD) {
		printOperandtoF(fp, code->u.binop.result);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.binop.op1);
		fprintf(fp, " + ");
		printOperandtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n");
	}
	else if (code->kind == SUB) {
		printOperandtoF(fp, code->u.binop.result);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.binop.op1);
		fprintf(fp, " - ");
		printOperandtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n");
	}
	else if (code->kind == MUL) {
		printOperandtoF(fp, code->u.binop.result);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.binop.op1);
		fprintf(fp, " * ");
		printOperandtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n");
	}
	else if (code->kind == DIV) {
		printOperandtoF(fp, code->u.binop.result);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.binop.op1);
		fprintf(fp, " / ");
		printOperandtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n");
	}
	else if (code->kind == LOAD) {
		printOperandtoF(fp, code->u.mem.result);
		fprintf(fp, " := *");
		printOperandtoF(fp, code->u.mem.addr);
		fprintf(fp, "\n");
	}
	else if (code->kind == STORE) {
		fprintf(fp, "*");
		printOperandtoF(fp, code->u.mem.addr);
		fprintf(fp, " := ");
		printOperandtoF(fp, code->u.mem.result);
		fprintf(fp, "\n");
	}
	else if (code->kind == BR) 
		fprintf(fp, "GOTO label%d\n", code->u.labelNo);
	else if (code->kind == Brelop) {
		fprintf(fp, "IF ");
		printOperandtoF(fp, code->u.brelop.x);
		switch (code->u.brelop.relop) {
			case equal: fprintf(fp, " == "); break;
			case notEqual: fprintf(fp, " != "); break;
			case greater: fprintf(fp, " > "); break;
			case greaterEqual: fprintf(fp, " >= "); break;
			case less: fprintf(fp, " < "); break;
			case lessEqual: fprintf(fp, " <= "); break;
		}
		printOperandtoF(fp, code->u.brelop.y);
		fprintf(fp, "\t GOTO label%d\n", code->u.brelop.labelNo);
	}
	else if (code->kind == DEC) {
		fprintf(fp, "DEC ");
		printOperandtoF(fp, code->u.dec.array);
		fprintf(fp, " %d\n", code->u.dec.size);
	}
	else if (code->kind == CALL) {
		printOperandtoF(fp, code->u.call.result);
		fprintf(fp, " := CALL %s\n", code->u.call.funcName);
	}
	else if (code->kind == RETURN) {
		fprintf(fp, "RETURN ");
		printOperandtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == ARG) {
		fprintf(fp, "ARG ");
		printOperandtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == PARAM) {
		fprintf(fp, "PARAM ");
		printOperandtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == READ) {
		fprintf(fp, "READ ");
		printOperandtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == WRITE) {
		fprintf(fp, "WRITE ");
		printOperandtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
}

void printICTable(struct ICNode *p, char* s)
{
//	printf("printICTable\n");
	struct ICNode *t = p;
/*	printIC(t->code);
	t = t->next;
	while(t != p) {
		printIC(t->code);
		t = t->next;
	}*/

	FILE *fp = fopen(s, "w");
	if (fp == NULL)
	{
		printf("file can't open!\n");
		return ;
	}
	t = p;
	printICtoF(fp, t->code);
	t = t->next;
	while(t != p) {
		printICtoF(fp, t->code);
		t = t->next;
	}
}


void translate(struct HashEntry ht[], struct TNode* r)
{
//	printf("%s!!!\n", r->name);
	if (strcmp(r->name, "ExtDef") == 0) {	//ExtDef: global variable, struct, function
		translateExtDef(r, ht);
	}
	if (r->firstChild != NULL) {
//		printf("	into %s firstChild\n", r->name);
		translate(ht, r->firstChild);
	}
	if (r->nextSibling != NULL) {
//		printf("	into %s nextSibling\n", r->name);
		translate(ht, r->nextSibling);
	}	
}


void translateExtDef(struct TNode *ExtDef, struct HashEntry ht[]) {
	if (strcmp(ExtDef->firstChild->nextSibling->name, "ExtDecList") == 0) 
		translateExtDecList(ExtDef->firstChild->nextSibling, ht);
	else if (strcmp(ExtDef->firstChild->nextSibling->name, "FunDec") == 0) {
		translateFunDec(ExtDef->firstChild->nextSibling, ht);
		translateCompSt(ExtDef->firstChild->nextSibling->nextSibling, ht);
	}	
}

void translateExtDecList(struct TNode *ExtDecList, struct HashEntry ht[]) {
	if (ExtDecList->firstChild->nextSibling == NULL)
		translateVarDec(ExtDecList->firstChild, ht, NULL);
	else {
		translateVarDec(ExtDecList->firstChild, ht, NULL);
		translateExtDecList(ExtDecList->firstChild->nextSibling->nextSibling, ht);
	}
}

struct Operand* translateVarDec(struct TNode *VarDec, struct HashEntry ht[], struct Operand* place) {
	if (strcmp(VarDec->firstChild->name, "ID") == 0) {
		struct LinkNode *n = HashTableSearch(ht, 16384, VarDec->firstChild->str);
		if (place == NULL) 
			n->op = OpVarInit();
		else
			n->op = place;
		return n->op;
	}
	else {//数组！
		if (place == NULL) place = OpArryInit();
		place = translateVarDec(VarDec->firstChild, ht, place);
		place->u.array.size = VarDec->firstChild->nextSibling->nextSibling->x * 4;
		ICTableInsert(IChead, ICNodeDecInit(DEC, place, place->u.array.size));
		return place;
	}
}

void translateFunDec(struct TNode *FunDec, struct HashEntry ht[]) {
	if (strcmp(FunDec->firstChild->nextSibling->nextSibling->name, "VarList") == 0) {
		struct LinkNode *n = HashTableSearch(ht, 16384, FunDec->firstChild->str);
		int i = 0;
		struct LinkNode *params = n->type->u.function.paras;
		while(params != NULL) {
			i++;
			params = params->n_level;
		}
		ICTableInsert(IChead, ICNodeFuncInit(FUNC, n->name, i));
		translateVarList(FunDec->firstChild->nextSibling->nextSibling, ht);
	}
	else {
		struct LinkNode *n = HashTableSearch(ht, 16384, FunDec->firstChild->str);
		ICTableInsert(IChead, ICNodeFuncInit(FUNC, n->name, 0));
	}
}

void translateVarList(struct TNode *VarList, struct HashEntry ht[]) {
	if (VarList->firstChild->nextSibling == NULL)
		translateParamDec(VarList->firstChild, ht);
	else {
		translateParamDec(VarList->firstChild, ht);
		translateVarList(VarList->firstChild->nextSibling->nextSibling, ht);
	}
}

void translateParamDec(struct TNode *ParamDec, struct HashEntry ht[]) {
	struct Operand* op = translateVarDec(ParamDec->firstChild->nextSibling, ht, NULL);
	ICTableInsert(IChead, ICNodeRWInit(PARAM, op));
}

struct ICNode* translateCompSt(struct TNode *CompSt, struct HashEntry ht[]) {
	struct ICNode* code1 = translateDefList(CompSt->firstChild->nextSibling, ht);
	struct ICNode* code2 = translateStmtList(CompSt->firstChild->nextSibling->nextSibling, ht);
	if (code1 == NULL) return code2;
	return code1;
}

struct ICNode* translateStmtList(struct TNode *StmtList, struct HashEntry ht[]) {
	if (strcmp(StmtList->firstChild->name, "Stmt") == 0) {
		struct ICNode* code1 = translateStmt(StmtList->firstChild, ht);
		struct ICNode* code2 = translateStmtList(StmtList->firstChild->nextSibling, ht);
		return code1;
	}
	return NULL;
}

struct ICNode* translateStmt(struct TNode *Stmt, struct HashEntry ht[])
{
	if (strcmp(Stmt->firstChild->name, "Exp") == 0) {
		struct Operand *t1 = OpTmpInit();
//		struct Operand *t1 = NULL;
		return translateExp(Stmt->firstChild, ht, t1);
	}
	else if (strcmp(Stmt->firstChild->name, "CompSt") == 0) {
		return translateCompSt(Stmt->firstChild, ht);	
	}
	else if (strcmp(Stmt->firstChild->name, "RETURN") == 0) {
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateExp(Stmt->firstChild->nextSibling, ht, t1);
		struct ICNode* code2 = ICNodeRWInit(RETURN, t1);
		ICTableInsert(IChead, code2);
		return code1;
	}
	else if (strcmp(Stmt->firstChild->name, "IF") == 0 && Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling == NULL) {
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label2 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* code1 = translateCond(Stmt->firstChild->nextSibling->nextSibling, label1, label2, ht);
		ICTableInsert(IChead, label1);
		struct ICNode* code2 = translateStmt(Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, ht);
		ICTableInsert(IChead, label2);
		return code1;
	}
	else if (strcmp(Stmt->firstChild->name, "IF") == 0 && Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL) {
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label2 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label3 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* code1 = translateCond(Stmt->firstChild->nextSibling->nextSibling, label1, label2, ht);
		ICTableInsert(IChead, label1);
		struct ICNode* code2 = translateStmt(Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, ht);
		ICTableInsert(IChead, ICNodeLabelInit(BR, labelNo));
		ICTableInsert(IChead, label2);
		struct ICNode* code3 = translateStmt(Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling, ht);
		ICTableInsert(IChead, label3);
		return code1;
	}
	else if (strcmp(Stmt->firstChild->name, "WHILE") == 0) {
		int n = labelNo+1;
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label2 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label3 = ICNodeLabelInit(LABEL, ++labelNo);
		ICTableInsert(IChead, label1);
		struct ICNode* code1 = translateCond(Stmt->firstChild->nextSibling->nextSibling, label2, label3, ht);
		ICTableInsert(IChead, label2);
		struct ICNode* code2 = translateStmt(Stmt->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, ht);
		ICTableInsert(IChead, ICNodeLabelInit(BR, n));
		ICTableInsert(IChead, label3);
		return label1;				
	}
}

struct ICNode* translateDefList(struct TNode *DefList, struct HashEntry ht[]) {
	if (strcmp(DefList->firstChild->name, "Def") == 0) {
		struct ICNode* code1 = translateDef(DefList->firstChild, ht);
		struct ICNode* code2 = translateDefList(DefList->firstChild->nextSibling, ht);
		if (code1 == NULL) return code2;
	}
	return NULL;
}

struct ICNode* translateDef(struct TNode *Def, struct HashEntry ht[]) {
	return translateDecList(Def->firstChild->nextSibling, ht);
}

struct ICNode* translateDecList(struct TNode *DecList, struct HashEntry ht[]) {
	if (DecList->firstChild->nextSibling == NULL)
		return translateDec(DecList->firstChild, ht);
	else {
		struct ICNode* code1 = translateDec(DecList->firstChild, ht);
		struct ICNode* code2 = translateDecList(DecList->firstChild->nextSibling->nextSibling, ht);
		if (code1 == NULL) return code2;
		return code1;
	}	
}

struct ICNode* translateDec(struct TNode *Dec, struct HashEntry ht[]) {
	if (Dec->firstChild->nextSibling == NULL) {
		translateVarDec(Dec->firstChild, ht, NULL);
		return NULL;
	}
	else {
		struct Operand* op = translateVarDec(Dec->firstChild, ht, NULL);
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code = translateExp(Dec->firstChild->nextSibling->nextSibling, ht, t1);
		ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, op, t1));
		return code;
	}	
}

struct ICNode* translateExp(struct TNode *Exp, struct HashEntry ht[], struct Operand *place)
{
//	printf("translateExp~~~%s\n", Exp->firstChild->name);
	if (strcmp(Exp->firstChild->name, "INT") == 0) {	//INT
		struct Operand *c = OpConstInit(Exp->firstChild->x);
		return ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, c));
	}
	else if (strcmp(Exp->firstChild->name, "ID") == 0 && Exp->firstChild->nextSibling == NULL) {	//ID
		struct LinkNode *n = HashTableSearch(ht, 16384, Exp->firstChild->str);
		return ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, n->op));
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "ASSIGNOP") == 0) {
		if (strcmp(Exp->firstChild->firstChild->name, "ID") == 0) {//左边是ID
			struct Operand *t1 = OpTmpInit();
			struct LinkNode *n = HashTableSearch(ht, 16384, Exp->firstChild->firstChild->str);
			struct ICNode* code1 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t1);
			ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, n->op, t1));
			ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, n->op));
			return code1;
		}
		else if (strcmp(Exp->firstChild->firstChild->nextSibling->name, "LB") == 0) {
			struct Operand *a1 = OpAddrInit(0);
			struct Operand *t1 = OpTmpInit();	
			struct ICNode* code1 = translateExp(Exp->firstChild, ht, a1);	
			struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t1);	
			ICTableInsert(IChead, ICNodeSolopInit(STORE, a1, t1));
			return code1;	
		}
	}		
	else if	(strcmp(Exp->firstChild->nextSibling->name, "PLUS") == 0) {	//+
		struct Operand *t1 = OpTmpInit();
		struct Operand *t2 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);
		struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t2);
		struct ICNode* code3 = ICTableInsert(IChead, ICNodeBinopInit(ADD, place, t1, t2));
		return code1;
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "MINUS") == 0) {	//-
		struct Operand *t1 = OpTmpInit();
		struct Operand *t2 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);
		struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t2);
		struct ICNode* code3 = ICTableInsert(IChead, ICNodeBinopInit(SUB, place, t1, t2));
		return code1;
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "STAR") == 0) {	//*
		struct Operand *t1 = OpTmpInit();
		struct Operand *t2 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);
		struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t2);
		struct ICNode* code3 = ICTableInsert(IChead, ICNodeBinopInit(MUL, place, t1, t2));
		return code1;
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "DIV") == 0) {	// div
		struct Operand *t1 = OpTmpInit();
		struct Operand *t2 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);
		struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t2);
		struct ICNode* code3 = ICTableInsert(IChead, ICNodeBinopInit(DIV, place, t1, t2));
		return code1;
	}
	else if (strcmp(Exp->firstChild->name, "MINUS") == 0) {			//-X
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild->nextSibling, ht, t1);
		struct ICNode* code2 = ICTableInsert(IChead, ICNodeBinopInit(SUB, place, OpConstInit(0), t1));
		return code1;
	}
	else if (strcmp(Exp->firstChild->name, "LP") == 0) {			//(X)
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild->nextSibling, ht, t1);
		struct ICNode* code2 = ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, t1));
		return code1;
	}	
	else if (strcmp(Exp->firstChild->nextSibling->name, "RELOP") == 0 
		|| strcmp(Exp->firstChild->name, "NOT") == 0
		|| strcmp(Exp->firstChild->nextSibling->name, "AND") == 0
		|| strcmp(Exp->firstChild->nextSibling->name, "OR") == 0) {	//
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* label2 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* code0 = ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, OpConstInit(0)));
		struct ICNode* code1 = translateCond(Exp, label1, label2, ht);
		struct ICNode* code2 = ICTableInsert(IChead, label1);
		ICTableInsert(IChead, ICNodeAssignInit(ASSIGN, place, OpConstInit(1)));
		ICTableInsert(IChead, label2);
		return code0;
	}

	else if (strcmp(Exp->firstChild->name, "ID") == 0 && strcmp(Exp->firstChild->nextSibling->nextSibling->name, "RP") == 0) {
		struct LinkNode *n = HashTableSearch(ht, 16384, Exp->firstChild->str);
		if (strcmp(n->name, "read") == 0)
			return ICTableInsert(IChead, ICNodeRWInit(READ, place));
		return ICTableInsert(IChead, ICNodeCallInit(CALL, place, n->name));
	}
	else if (strcmp(Exp->firstChild->name, "ID") == 0 && strcmp(Exp->firstChild->nextSibling->nextSibling->name, "Args") == 0) {
		struct LinkNode *n = HashTableSearch(ht, 16384, Exp->firstChild->str);
		struct Operand *argList = (struct Operand*)malloc(sizeof(struct Operand));
		struct ICNode* code1 = translateArgs(Exp->firstChild->nextSibling->nextSibling, ht, argList);
		if (strcmp(n->name, "write") == 0) {
			ICTableInsert(IChead, ICNodeRWInit(WRITE, argList->next));
			return code1;
		}
		struct Operand *arg = argList->next;
		while (arg != NULL) {
			ICTableInsert(IChead, ICNodeRWInit(ARG, arg));
			arg = arg->next;
		}
		return ICTableInsert(IChead, ICNodeCallInit(CALL, place, n->name));
	}

	else if (strcmp(Exp->firstChild->nextSibling->name, "LB") == 0) {	//array
		if (strcmp(Exp->firstChild->firstChild->name, "ID") == 0) {
			struct LinkNode *n = HashTableSearch(ht, 16384, Exp->firstChild->firstChild->str);
			struct Operand *a1 = OpAddrInit(n->op->u.varNo);
			struct Operand *t1 = OpTmpInit();
			struct ICNode* code1 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t1);
			struct Operand *c1 = OpConstInit(4);
			ICTableInsert(IChead, ICNodeBinopInit(MUL, t1, t1, c1));
			if (place->kind == ADDRESS) {
				place->kind = TEMPV;
				place->u.tmpNo = ++tempVNo;
				ICTableInsert(IChead, ICNodeBinopInit(ADD, place, a1, t1));
			}
			else {
				struct Operand *t2 = OpTmpInit();
				ICTableInsert(IChead, ICNodeBinopInit(ADD, t2, a1, t1));
				ICTableInsert(IChead, ICNodeMemInit(LOAD, t2, place));
			}
			return code1;
		}
	}
}

struct ICNode* translateArgs(struct TNode *Args, struct HashEntry ht[], struct Operand *argList) 
{
	if (strcmp(Args->firstChild->name, "Exp") == 0 && Args->firstChild->nextSibling == NULL) {
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateExp(Args->firstChild, ht, t1);
		if (argList == NULL) argList = t1;
		else {
			while(argList->next != NULL)
				argList = argList->next;
			argList->next = t1;
		}
		return code1;		
	}
	else if (strcmp(Args->firstChild->name, "Exp") == 0 && Args->firstChild->nextSibling != NULL) {
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateArgs(Args->firstChild->nextSibling->nextSibling, ht, argList);
		if (argList == NULL) argList = t1;
		else {
			struct Operand *arg = argList;
			while(arg->next != NULL)
				arg = arg->next;
			arg->next = t1;
		}
		struct ICNode* code2 = translateExp(Args->firstChild, ht, t1);
		return code1;
	}
}

struct ICNode* translateCond(struct TNode *Exp, struct ICNode *labelTrue, struct ICNode *labelFalse, struct HashEntry ht[])
{
//	printf("translateCond!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	if (Exp->firstChild->nextSibling == NULL) {
		struct Operand *t1 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);		
		struct ICNode* code2 = ICNodeBrelopInit(Brelop, t1, OpConstInit(0), notEqual, labelTrue->code->u.labelNo);
		ICTableInsert(IChead, code2);
		ICTableInsert(IChead, ICNodeLabelInit(BR, labelFalse->code->u.labelNo));
		return code1;
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "RELOP") == 0) {
		struct Operand *t1 = OpTmpInit();
		struct Operand *t2 = OpTmpInit();
		struct ICNode* code1 = translateExp(Exp->firstChild, ht, t1);
		struct ICNode* code2 = translateExp(Exp->firstChild->nextSibling->nextSibling, ht, t2);
		struct ICNode* code3 = ICNodeBrelopInit(Brelop, t1, t2, Exp->firstChild->nextSibling->relop, labelTrue->code->u.labelNo);
		ICTableInsert(IChead, code3);
		ICTableInsert(IChead, ICNodeLabelInit(BR, labelFalse->code->u.labelNo));
	}
	else if (strcmp(Exp->firstChild->name, "NOT") == 0) {
		return translateCond(Exp, labelFalse, labelTrue, ht);
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "AND") == 0) {
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* code1 = translateCond(Exp->firstChild, label1, labelFalse, ht);
		ICTableInsert(IChead, label1);	
		struct ICNode* code2 = translateCond(Exp->firstChild->nextSibling->nextSibling, labelTrue, labelFalse, ht);
		return code1;
	}
	else if (strcmp(Exp->firstChild->nextSibling->name, "OR") == 0) {
		struct ICNode* label1 = ICNodeLabelInit(LABEL, ++labelNo);
		struct ICNode* code1 = translateCond(Exp->firstChild, labelTrue, label1, ht);
		ICTableInsert(IChead, label1);	
		struct ICNode* code2 = translateCond(Exp->firstChild->nextSibling->nextSibling, labelTrue, labelFalse, ht);
		return code1;
	}

}


