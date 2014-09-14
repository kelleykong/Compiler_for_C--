#include "ICGen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "IDList.h"

extern int varNo;
extern int tempVNo;
int paramCount = 0;
int paramNum = 0;

void printRegtoF(FILE *fp, struct Operand* op)
{
	if (op->kind == VARIABLE)
		fprintf(fp, "v%d", op->u.varNo);
	else if (op->kind == ADDRESS)
		fprintf(fp, "v%d", op->u.addr);
	else if (op->kind == TEMPV)
		fprintf(fp, "t%d", op->u.varNo);
	else if (op->kind == CONSTANT)
		fprintf(fp, "%d", op->u.value);
	else if (op->kind == ARRAY)
		fprintf(fp, "v%d", op->u.array.varNo);
}

int marklive(struct ICNode *node, int *mark)
{
	struct ICNode *n = node;
	int num = 0;
	while(n->code->kind != RETURN) {
		if (n->code->kind == ASSIGN) {
			if (n->code->u.assign.right->kind == VARIABLE) {
				mark[n->code->u.assign.right->u.varNo-1] = 1;
				num++;
			}
			else if (n->code->u.assign.left->kind == VARIABLE) {
				mark[n->code->u.assign.left->u.varNo-1] = 1;
				num++;
			}
		}
		n = n->next;
	}
	return num;
}

void printTCtoF(FILE *fp, struct ICNode *n)
{
	struct InterCode *code = n->code;
	if (code->kind == LABEL)
		fprintf(fp, "label%d:\n", code->u.labelNo);
	else if (code->kind == FUNC) {
		fprintf(fp, "\n%s:\n", code->u.func.funcName);
		paramCount = code->u.func.paramNum;
		paramNum = paramCount;
	}
	else if (code->kind == ASSIGN) {
		if (code->u.assign.right->kind == CONSTANT) {//t = #立即数
			fprintf(fp, "\tli $t1, ");
			printRegtoF(fp, code->u.assign.right);
			fprintf(fp, "\n\tsw $t1, ");
			printRegtoF(fp, code->u.assign.left);
			fprintf(fp, "\n");
		}
		else if (code->u.assign.right->kind == VARIABLE) {//t = v(m)
			fprintf(fp, "\tlw $t1, ");
			printRegtoF(fp, code->u.assign.right);
			fprintf(fp, "\n\tsw $t1, ");
			printRegtoF(fp, code->u.assign.left);
			fprintf(fp, "\n");
		}
		else if (code->u.assign.left->kind == VARIABLE) {//v(m) = t
			fprintf(fp, "\tlw $t1, ");
			printRegtoF(fp, code->u.assign.right);
			fprintf(fp, "\n\tsw $t1, ");
			printRegtoF(fp, code->u.assign.left);
			fprintf(fp, "\n");
		}
		else if (code->u.assign.right->kind == ADDRESS) {//t = addr
			fprintf(fp, "\tla $t1, ");
			printRegtoF(fp, code->u.assign.right);
			fprintf(fp, "\n\tsw $t1, ");
			printRegtoF(fp, code->u.assign.left);
			fprintf(fp, "\n");
		}
		else {
			fprintf(fp, "\tlw $t1, ");
			printRegtoF(fp, code->u.assign.right);
			fprintf(fp, "\n\tsw $t1, ");
			printRegtoF(fp, code->u.assign.left);
			fprintf(fp, "\n");
		}
	}
	else if (code->kind == ADD) {//没有立即数 t = t + t
		if (code->u.binop.op1->kind == ADDRESS) {
			fprintf(fp, "\tla $t1, ");
			printRegtoF(fp, code->u.binop.op1);
			fprintf(fp, "\n\tlw $t2, ");
			printRegtoF(fp, code->u.binop.op2);
			fprintf(fp, "\n\tadd $t3, $t1, $t2\n\tsw $t3, ");
			printRegtoF(fp, code->u.binop.result);
			fprintf(fp, "\n");
		}
		else {
			fprintf(fp, "\tlw $t1, ");
			printRegtoF(fp, code->u.binop.op1);
			fprintf(fp, "\n\tlw $t2, ");
			printRegtoF(fp, code->u.binop.op2);
			fprintf(fp, "\n\tadd $t3, $t1, $t2\n\tsw $t3, ");
			printRegtoF(fp, code->u.binop.result);
			fprintf(fp, "\n");
		}
	}
	else if (code->kind == SUB) {
		if (code->u.binop.op1->kind == CONSTANT) 
			fprintf(fp, "\tli $t1, ");
		else
			fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.binop.op1);
		fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n\tsub $t3, $t1, $t2\n\tsw $t3, ");
		printRegtoF(fp, code->u.binop.result);
		fprintf(fp, "\n");
	}
	else if (code->kind == MUL) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.binop.op1);
		if (code->u.binop.op2->kind == CONSTANT) 
			fprintf(fp, "\n\tli $t2, ");
		else
			fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n\tmul $t3, $t1, $t2\n\tsw $t3, ");
		printRegtoF(fp, code->u.binop.result);
		fprintf(fp, "\n");
	}
	else if (code->kind == DIV) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.binop.op1);
		fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.binop.op2);
		fprintf(fp, "\n\tdiv $t3, $t1, $t2\n\tsw $t3, ");
		printRegtoF(fp, code->u.binop.result);
		fprintf(fp, "\n");
	}
	else if (code->kind == LOAD) {
		fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.mem.addr);
		fprintf(fp, "\n\tlw $t1, 0($t2)\n");
		fprintf(fp, "\tsw $t1, ");
		printRegtoF(fp, code->u.mem.result);
		fprintf(fp, "\n");
	}
	else if (code->kind == STORE) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.mem.result);
		fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.mem.addr);
		fprintf(fp, "\n\tsw $t1, 0($t2)\n");
	}
	else if (code->kind == BR) 
		fprintf(fp, "\tj label%d\n", code->u.labelNo);
	else if (code->kind == Brelop) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.brelop.x);
		fprintf(fp, "\n\tlw $t2, ");
		printRegtoF(fp, code->u.brelop.y);
		switch (code->u.brelop.relop) {
			case equal: fprintf(fp, "\n\tbeq"); break;
			case notEqual: fprintf(fp, "\n\tbne"); break;
			case greater: fprintf(fp, "\n\tbgt"); break;
			case greaterEqual: fprintf(fp, "\n\tbge"); break;
			case less: fprintf(fp, "\n\tblt"); break;
			case lessEqual: fprintf(fp, "\n\tble"); break;
		}
		fprintf(fp, " $t1, $t2, label%d\n", code->u.brelop.labelNo);
	}
	else if (code->kind == DEC) {//?
/*		fprintf(fp, "DEC ");
		printRegtoF(fp, code->u.dec.array);
		fprintf(fp, " %d\n", code->u.dec.size);
*/	}
	else if (code->kind == CALL) {
		int *mark = (int*)malloc(4*varNo);
		memset(mark, 0, varNo);
		int num = marklive(n, mark);
		fprintf(fp, "\tmove $fp, $sp\n");
		fprintf(fp, "\taddi $sp, $sp, -4\n");
		fprintf(fp, "\tsw $ra, 0($sp)\n");
		int i = num, j = 0;
		while (i > 0) {
			if (mark[j] == 1) {
				fprintf(fp, "\tlw $t1, v%d\n", j+1);
				fprintf(fp, "\taddi $sp, $sp, -4\n");
				fprintf(fp, "\tsw $t1, 0($sp)\n");
				i--;
			}
			j++;
		}
		fprintf(fp, "\tjal %s\n", code->u.call.funcName);
		while(i < num) {
			if (mark[j] == 1) {
				fprintf(fp, "\tlw $t1, 0($sp)\n");
				fprintf(fp, "\taddi $sp, $sp, 4\n");
				fprintf(fp, "\tsw $t1, v%d\n", j+1);
				i++;
			}
			j--;
		}
		fprintf(fp, "\tlw $ra, 0($sp)\n");
		fprintf(fp, "\taddi $sp, $sp, 4\n");
		fprintf(fp, "\tsw $v0, ");
		printRegtoF(fp, code->u.call.result);
		fprintf(fp, "\n\taddi $sp, $sp, %d\n", 4*(paramNum-4));
		paramCount = 0;
	}
	else if (code->kind == RETURN) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.rw.op);
		fprintf(fp, "\n\tmove $v0, $t1\n");
		fprintf(fp, "\tjr $ra\n");
	}
	else if (code->kind == ARG) {//...?
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.rw.op);
		if (paramCount < 4) 
			fprintf(fp, "\n\tmove $a%d, $t1\n", paramCount++);
		else {
			fprintf(fp, "\n\taddi $sp, $sp, -4\n");
			fprintf(fp, "\tsw $t1, 0($sp)\n");
			paramCount++;
		}
	}
	else if (code->kind == PARAM) {//...?
		if (paramCount > 4) {
			fprintf(fp, "\tlw $t1, 0($fp)\n");
			fprintf(fp, "\taddi $fp, $fp, 4\n");
			paramCount--;
		}
		else 
			fprintf(fp, "\tmove $t1, $a%d\n", --paramCount);		
		fprintf(fp, "\tsw $t1, ");
		printRegtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == READ) {
		fprintf(fp, "\taddi $sp, $sp, -4\n");
		fprintf(fp, "\tsw $ra, 0($sp)\n");
		fprintf(fp, "\tjal read\n");
		fprintf(fp, "\tlw $ra, 0($sp)\n");
		fprintf(fp, "\taddi $sp, $sp, 4\n");
		fprintf(fp, "\tsw $v0, ");
		printRegtoF(fp, code->u.rw.op);
		fprintf(fp, "\n");
	}
	else if (code->kind == WRITE) {
		fprintf(fp, "\tlw $t1, ");
		printRegtoF(fp, code->u.rw.op);
		fprintf(fp, "\n\tmove $a0, $t1\n");
		fprintf(fp, "\taddi $sp, $sp, -4\n");
		fprintf(fp, "\tsw $ra, 0($sp)\n");
		fprintf(fp, "\tjal write\n");
		fprintf(fp, "\tlw $ra, 0($sp)\n");
		fprintf(fp, "\taddi $sp, $sp, 4\n");
	}
}

void TCInitial(FILE* fp, struct ICNode *p)
{
	fprintf(fp, ".data\n");
	fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(fp, "_ret: .asciiz \"\\n\"\n");
	int *mark = (int*)malloc(4*varNo);
	memset(mark, 0, varNo);
	struct ICNode *t = p;
	if (t->code->kind == DEC) {
		if (t->code->u.dec.array->kind == ARRAY)
			mark[t->code->u.dec.array->u.varNo-1] = 1;
	}
	t = t->next;
	while(t != p) {
		if (t->code->kind == DEC) {
			if (t->code->u.dec.array->kind == ARRAY)
				mark[t->code->u.dec.array->u.varNo-1] = t->code->u.dec.size;
		}
		t = t->next;
	}	
	int i;
	for(i = 1; i < varNo+1; i++) {
		if (mark[i-1] != 0)
			fprintf(fp, "v%d: .space %d\n", i, mark[i-1]);
		else
			fprintf(fp, "v%d: .word 0\n", i);
	}
	for(i = 1; i < tempVNo+1; i++) {
		fprintf(fp, "t%d: .word 0\n", i);
	}
	fprintf(fp, ".globl main\n");
	fprintf(fp, ".text\n");
	fprintf(fp, "read:\n");
	fprintf(fp, "\tli $v0, 4\n");
	fprintf(fp, "\tla $a0, _prompt\n");
	fprintf(fp, "\tsyscall\n");
	fprintf(fp, "\tli $v0, 5\n");
	fprintf(fp, "\tsyscall\n");
	fprintf(fp, "\tjr $ra\n\n");
	fprintf(fp, "write:\n");
	fprintf(fp, "\tli $v0, 1\n");
	fprintf(fp, "\tsyscall\n");
	fprintf(fp, "\tli $v0, 4\n");
	fprintf(fp, "\tla $a0, _ret\n");
	fprintf(fp, "\tsyscall\n");
	fprintf(fp, "\tmove $v0, $0\n");
	fprintf(fp, "\tjr $ra\n\n");
}
void genTargetCode(struct ICNode *p, char* s) 
{
	printf("genTargetCode\n");
	struct ICNode *t = p;
/*	printTC(t->code);
	t = t->next;
	while(t != p) {
		printTC(t->code);
		t = t->next;
	}*/

	FILE *fp = fopen(s, "w");
	if (fp == NULL)
	{
		printf("file can't open!\n");
		return ;
	}
	TCInitial(fp, p);
	t = p;
	printTCtoF(fp, t);
	t = t->next;
	while(t != p) {
		printTCtoF(fp, t);
		t = t->next;
	}
}
