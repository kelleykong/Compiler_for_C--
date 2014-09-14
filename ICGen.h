#include "Tree.h"
struct Operand {
	enum {VARIABLE, CONSTANT, ADDRESS, TEMPV, ARRAY } kind;
	union {
		int varNo;	
		int value;
		int addr;
		int tmpNo;
		struct {int varNo, size; } array;
	} u;
	struct Operand* next;
};

struct InterCode {
	enum {LABEL, FUNC, ASSIGN, ADD, SUB, MUL, DIV, LOAD, STORE,
		BR, Brelop, DEC, CALL, RETURN, ARG, PARAM, READ, WRITE} kind;
	union {
		int labelNo;	//label, br
		struct {char* funcName; int paramNum; } func;	//func
		struct {struct Operand *right, *left; } assign;//assign, (x)
		struct {struct Operand *result, *op1, *op2; } binop;//+,-,*,\,-x
		struct {struct Operand *result, *op; } solop;//not
		struct {struct Operand *addr, *result; } mem;//load, store
		struct {struct Operand *x, *y; enum Relop relop; int labelNo; } brelop;
		struct {struct Operand *array; int size; } dec;
		struct {struct Operand *result; char* funcName; } call;	//call
		struct {struct Operand *op; } rw;//read, write, return, arg, param, 
	} u;
};

struct ICNode {
	struct InterCode *code;
	struct ICNode *prev, *next; 
};
