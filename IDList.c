#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"
#include "IDList.h"

void dealExtDef(struct HashEntry ht[], int sz, struct TNode* p);
struct Type* dealSpecifier(struct HashEntry ht[], int sz, struct TNode *p);
struct LinkNode* dealOptTag(struct HashEntry ht[], int sz, struct TNode *p);
void dealExtDecList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s);
struct LinkNode* dealDefList(struct HashEntry ht[], int sz,  struct TNode* p, int is_s);
struct LinkNode* dealDef(struct HashEntry ht[], int sz,  struct TNode* p, int is_s);
struct LinkNode* dealDecList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s);
struct LinkNode* dealDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s);
struct LinkNode* dealVarDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s);
void dealFunDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s);
struct LinkNode* dealVarList(struct HashEntry ht[], int sz, struct TNode *p);
struct LinkNode* dealParamDec(struct HashEntry ht[], int sz, struct TNode *p);
void dealCompSt(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s);
void dealStmtList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s);
void dealStmt(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s);
struct LinkNode* dealExp(struct HashEntry ht[], int sz, struct TNode *p);
struct LinkNode* dealArgs(struct HashEntry ht[], int sz, struct TNode *p, struct LinkNode *n);

int is_typeMatch(struct LinkNode* n1, struct LinkNode* n2, struct TNode *p1, struct TNode *p2);

void initHashTable(struct HashEntry ht[], int sz) {
	struct LinkNode* n1 = HashTableInsert(ht, sz, "read");
	n1->state = Func;
	n1->type = (struct Type*)malloc(sizeof(struct Type));
	n1->type->kind = function;
	struct Type* s = (struct Type*)malloc(sizeof(struct Type));
	s->kind = basic;
	s->u.basic = Integer;
	n1->type->u.function.retType = s; 
	n1->type->u.function.paraNum = 0;
	n1->type->u.function.paras = NULL;
	n1 = HashTableInsert(ht, sz, "write");
	n1->state = Func;
	n1->type = (struct Type*)malloc(sizeof(struct Type));
	n1->type->kind = function;
	s = (struct Type*)malloc(sizeof(struct Type));
	s->kind = basic;
	s->u.basic = Integer;
	n1->type->u.function.retType = s; 
	n1->type->u.function.paraNum = 1;
	struct LinkNode *node = (struct LinkNode *)malloc(sizeof(struct LinkNode));
	node->state = FuncParam;
	s = (struct Type*)malloc(sizeof(struct Type));
	s->kind = basic;
	s->u.basic = Integer;
	node->type = s;	
	n1->type->u.function.paras = node;

}

struct LinkNode* HashTableInsert(struct HashEntry ht[], int sz, char* e)
{
//	printf("insert name: %s\n", e);
	int i = hash_pjw(e);
//	printf("hash num: %d\n", i);
	struct LinkNode* node = HashTableSearch(ht, sz, e);

	if(node != NULL)
	{
//		printf("	the hashEntry has already exist!\n");
		return NULL;
	}
	else
	{
		struct LinkNode *node = (struct LinkNode *)malloc(sizeof(struct LinkNode));
		node->name = (char *)malloc(256);
		strcpy(node->name, e);
		if (ht[i].head != NULL)
			node->next = ht[i].head->next;
		ht[i].head = node;	
		ht[i].linkNum++;
//		printf("	%s insert in %x succeed!\n", e, i);
		return node;
	}
}

struct LinkNode* HashTableSearch(struct HashEntry ht[], int sz, char* e)	
{
//	printf("search name: %s\n", e);
	int i = hash_pjw(e);

	if (ht[i].linkNum != 0) {
		struct LinkNode* node = ht[i].head;
		while(strcmp(node->name, e) != 0)
		{
			if(node->next != NULL)
				node = node->next;
			else
				break;
		}
		if(node == NULL) {
//			printf("	can't find %s\n", e);
			return NULL;
		}
		else {
//			printf("	find %s in %x\n", e, i);
			return node;
		}
	}
	else {
//		printf("	can't find %s\n", e);
		return NULL;
	}

}

int HashTableDelete(struct HashEntry ht[], int sz, char *e)	//success, return -1; fail, return hashNum
{
//	printf("delete name: %s\n", e);
	int i = hash_pjw(e);
	struct LinkNode* node = HashTableSearch(ht, sz, e);

	if(node != NULL)
	{
		struct LinkNode* node = ht[i].head;
		if(strcmp(node->name, e) == 0) {
			ht[i].head = node->next;
			free(node);
			ht[i].linkNum--;
//			printf("	delete %s in i\n", e, i);
			return -1;
		}
		else {
			while(node->next != NULL && strcmp(node->next->name, e) != 0)
				node = node->next;		
			struct LinkNode *p = node->next;
			node->next = p->next;
			free(p);
			ht[i].linkNum--;
//			printf("	delete %s in i\n", e, i);
			return -1;
		}
	}
	else
	{
//		printf("	delete %s failed\n", e);
		return i;
	}
}

unsigned int hash_pjw(char* name)
{
	unsigned int val = 0, i;
	for( ; *name; ++name)
	{
		val = (val << 2) + *name;
//		printf("val<<2+*name: %x\n", val);
		if (i = val & ~0x3fff) {
			val = (val ^ (i >> 12)) & 0x3fff;
//			printf("i>>12: %x\n", i>>12);
		}
//		printf("i: %x, val: %x\n", i, val);
	}
	return val;
}


void checkStruct(struct HashEntry ht[], int sz, char *n)
{
	struct LinkNode* node = HashTableSearch(ht, sz, n);
//	printf("struct  %s:  %s\n", node->type->u.structure->name, node->name);
	node = node->type->u.structure->type->u.structure;
	while(node != NULL) {
//		printf("%s	", node->name);
		node = node->n_level;
	}
//	printf("\n");
}

void checkArray(struct HashEntry ht[], int sz, char *n)
{
//	printf("checkArray~~~%s\n", n);
	struct LinkNode* node = HashTableSearch(ht, sz, n);
//	printf("array  %s", node->name);
	struct Type* t = node->type;
	while(t->kind == array) {
//		printf("[%d] ", t->u.array.size);
		t = t->u.array.elem;
	}
	if (t->kind == structure) {
		node = t->u.structure;
		printf("	struct  %s:  \n", node->name);
		node = node->type->u.structure;
		while(node != NULL) {
			printf("%s	", node->name);
			node = node->n_level;
		}	
	}
	else {
		printf("	%d", t->u.basic);
	}
	printf("\n");
}

void checkFunc(struct HashEntry ht[], int sz, char *n)
{
	printf("checkFunction~~~%s\n", n);	
	struct LinkNode* node = HashTableSearch(ht, sz, n);
	printf("function:  %d  %s(", node->type->u.function.retType->u.basic, node->name);
	if (node->type->u.function.paras == NULL)
		printf("no parameters)\n");
	else {
		node = node->type->u.function.paras;
		while (node != NULL) {
			printf("%d %s, ",node->type->u.basic, node->name);
			node = node->n_level;
		}
	}
	printf(")\n");
	
}

void createIDList(struct HashEntry ht[], int sz, struct TNode* r)
{
//	printf("%s!!!\n", r->name);
	if (strcmp(r->name, "ExtDef") == 0) {	//ExtDef: global variable, struct, function
		dealExtDef(ht, sz, r);
	}
	if (r->firstChild != NULL) {
//		printf("	into %s firstChild\n", r->name);
		createIDList(ht, sz, r->firstChild);
	}
	if (r->nextSibling != NULL) {
//		printf("	into %s nextSibling\n", r->name);
		createIDList(ht, sz, r->nextSibling);
	}
}

void dealExtDef(struct HashEntry ht[], int sz, struct TNode* p)
{
//	printf("dealExtDef~~~%s\n", p->firstChild->firstChild->name);

	//ExtDef:Specifier...
	struct Type* specifier = dealSpecifier(ht, sz, p->firstChild);

	if (strcmp(p->firstChild->nextSibling->name, "ExtDecList") == 0) {//ExtDef:global variable
		dealExtDecList(ht, sz, p->firstChild->nextSibling, specifier);
	} else if (strcmp(p->firstChild->nextSibling->name, "FunDec") == 0) {//ExtDef:function
		dealFunDec(ht, sz, p->firstChild->nextSibling, specifier);
		dealCompSt(ht, sz, p->firstChild->nextSibling->nextSibling, specifier);
	} else {//ExtDef:struct
	}
	
}

struct Type* dealSpecifier(struct HashEntry ht[], int sz, struct TNode* p)
{
//	printf("dealSpecifier~~~%s\n", p->firstChild->name);
	struct Type *specifier = (struct Type*)malloc(sizeof(struct Type));
	if (strcmp(p->firstChild->name, "TYPE") == 0) {
		if (strcmp(p->firstChild->str, "int") == 0) {
//			printf("TYPE:int\n");
			specifier->kind = basic;
			specifier->u.basic = Integer;
		}
		else {
//			printf("TYPE:float\n");			
			specifier->kind = basic;
			specifier->u.basic = Float;
		}
	}
	else if (strcmp(p->firstChild->name, "StructSpecifier") == 0) {
		if (strcmp(p->firstChild->firstChild->nextSibling->name, "OptTag") == 0) {
//			printf("STRUCT Definition %s\n", p->firstChild->firstChild->nextSibling->firstChild->str);
			struct LinkNode *node = dealOptTag(ht, sz, p->firstChild->firstChild->nextSibling);
			if (node == NULL) {
				printf("Error type 16 at line%d: Struct name redefined \"%s\"\n", p->line, p->firstChild->firstChild->nextSibling->firstChild->str);
				return NULL;
			}
			node->state = Struct;
			node->type = (struct Type*)malloc(sizeof(struct Type));
			node->type->kind = structure;
			node->type->u.structure = dealDefList(ht, sz, p->firstChild->firstChild->nextSibling->nextSibling->nextSibling, 1);
/*			struct LinkNode* structFld = node->type->u.structure;
			if (structFld != NULL) {
				while (structFld != NULL) {
					structFld->state = StructField;
					structFld = structFld->n_level;
				}
			}*/
			specifier->kind = structure;
			specifier->u.structure = node;
//			printf("struct %s: %s	%s\n", node->name, node->type->u.structure->name, node->type->u.structure->n_level->name);
		}
		else {
			//Tag查表...?
//			printf("STRUCT %s\n", p->firstChild->firstChild->nextSibling->firstChild->str);
			struct LinkNode *node = HashTableSearch(ht, sz, p->firstChild->firstChild->nextSibling->firstChild->str);
			if (node == NULL || node->type->kind != structure) {
				printf("Error type 17 at %d: Struct undefined \"%s\"\n", p->line,  p->firstChild->firstChild->nextSibling->firstChild->str);
				return NULL;
			}
			specifier->kind = structure;
			specifier->u.structure = node;	//指向同一个结构体定义！..?
		}
	}
	return specifier;
}

struct LinkNode* dealOptTag(struct HashEntry ht[], int sz, struct TNode *p)
{
//	printf("dealoptTag~~~%s\n", p->firstChild->name);
	struct LinkNode *node;
	if (strcmp(p->firstChild->name, "ID") == 0) {
		node = HashTableInsert(ht, sz, p->firstChild->str);
//		node->state = Struct;	
	}
	else {//空...?
		node = HashTableInsert(ht, sz, p->firstChild->name);
//		node->state = Struct;
	}
	return node;
}	

void dealExtDecList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s)
{
//	printf("dealExtDecList~~~\n");
	struct LinkNode *node = dealVarDec(ht, sz, p->firstChild, s, 0);
//	node->state = Variable;
//	node->type = s;
	if (p->firstChild->nextSibling != NULL && 
		strcmp(p->firstChild->nextSibling->nextSibling->name, "ExtDecList") == 0)
		dealExtDecList(ht, sz, p->firstChild->nextSibling->nextSibling, s);
}

struct LinkNode* dealDefList(struct HashEntry ht[], int sz,  struct TNode* p, int is_s)
{
//	printf("dealDefList~~~\n");
	struct LinkNode* node = NULL;
	if (p->firstChild != NULL && strcmp(p->firstChild->name, "Def") == 0) {
		node = dealDef(ht, sz, p->firstChild, is_s);
		if (node != NULL) {
			struct LinkNode *t = node;		
			while(t->n_level != NULL) t = t->n_level;
			t->n_level = dealDefList(ht, sz, p->firstChild->nextSibling, is_s);
		}
		else
			node = dealDefList(ht, sz, p->firstChild->nextSibling, is_s);
	}
	return node;
}

struct LinkNode* dealDef(struct HashEntry ht[], int sz,  struct TNode* p, int is_s)
{
//	printf("dealDef~~~\n");

	//Def:Specifier...
	struct Type* specifier = dealSpecifier(ht, sz, p->firstChild);

	if (strcmp(p->firstChild->nextSibling->name, "DecList") == 0) {//Def:local variable
		return dealDecList(ht, sz, p->firstChild->nextSibling, specifier, is_s);
	}
}

struct LinkNode* dealDecList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s)
{
//	printf("dealDecList~~~\n");
	struct LinkNode *node = dealDec(ht, sz, p->firstChild, s, is_s);
//	node->state = Variable;
//	node->type = s;
	if (p->firstChild->nextSibling != NULL && 
		strcmp(p->firstChild->nextSibling->nextSibling->name, "DecList") == 0) {
		if (node != NULL) 
			node->n_level = dealDecList(ht, sz, p->firstChild->nextSibling->nextSibling, s, is_s);
		else
			node = dealDecList(ht, sz, p->firstChild->nextSibling->nextSibling, s, is_s);
	}
	return node;
}

struct LinkNode* dealDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s)
{
//	printf("dealDec~~~\n");
	struct LinkNode *n1 = dealVarDec(ht, sz, p->firstChild, s, is_s);
	if (n1 == NULL) return NULL;
	if (is_s == 1)
		n1->state = StructField;
//	node->state = Variable;
//	node->type = s;
//=Exp...?
	if (p->firstChild->nextSibling != NULL) {
		if (is_s == 1)
			printf("Error type 15 at line %d: \"%s\" Illegal initial in Struct\n", p->line, n1->name);
		struct LinkNode *n2 = dealExp(ht, sz, p->firstChild->nextSibling->nextSibling);
		if (n2 != NULL) {
			if (n1->type->kind == array) {//定义变量初始化 =左边不能是数组
				printf("Error type 5 at line %d: Type mismatched (=)\n", p->line);
				n2 = NULL;
			}
			if (is_typeMatch(n1, n2, p->firstChild, p->firstChild->nextSibling->nextSibling) == 0) {
				printf("Error type 5 at line %d: Type mismatched (=)\n", p->line);
				n2 = NULL;	
			}
		}		
	}
	return n1;
}

struct LinkNode* dealVarDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s, int is_s)
{
//	printf("dealVarDec~~~%s\n", p->firstChild->name);
	struct LinkNode *node;
	if (strcmp(p->firstChild->name, "ID") == 0) {
		node = HashTableInsert(ht, sz, p->firstChild->str);
		if (node == NULL) {
			if (is_s == 1) {
				printf("Error type 15 at line %d: StructField Variable redefined \"%s\"\n", p->firstChild->line, p->firstChild->str);
				return NULL;
			}
			printf("Error type 3 at %d: Variable redefined \"%s\"\n", p->firstChild->line, p->firstChild->str);
			return NULL;
		}
		node->state = Variable;
		node->type = s;	
		return node;	
	}
	else {//数组...?
		node = dealVarDec(ht, sz, p->firstChild, s, is_s);
		if (node == NULL) return NULL;
		node->state = Variable;
		struct Type* t = node->type;
		if (node->type->kind != array) {
			node->type = (struct Type*)malloc(sizeof(struct Type));
			node->type->kind = array;
			node->type->u.array.elem = t;
			node->type->u.array.size = p->firstChild->nextSibling->nextSibling->x;	
			return node;
		}		
		while(t->u.array.elem->kind == array) 
			t = t->u.array.elem;
		t->u.array.elem = (struct Type*)malloc(sizeof(struct Type));
		t->u.array.elem->kind = array;
		t->u.array.elem->u.array.elem = s;
		t->u.array.elem->u.array.size = p->firstChild->nextSibling->nextSibling->x;
	}
	return node;
}

void dealFunDec(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s)
{
	//ID
	struct LinkNode *node = HashTableInsert(ht, sz, p->firstChild->str);
	if (node == NULL) {
		printf("Error type 4 at %d: Function redefined \"%s\"\n", p->firstChild->line, p->firstChild->str);
		return ;
	}
	node->state = Func;
	node->type = (struct Type*)malloc(sizeof(struct Type));
	node->type->kind = function;
	node->type->u.function.retType = s;
	if (strcmp(p->firstChild->nextSibling->nextSibling->name, "RP") == 0) {
		node->type->u.function.paraNum = 0;
		node->type->u.function.paras = NULL;
	}
	else {
		node->type->u.function.paraNum = 1;
		node->type->u.function.paras = dealVarList(ht, sz, p->firstChild->nextSibling->nextSibling);
	}
}

struct LinkNode* dealVarList(struct HashEntry ht[], int sz, struct TNode *p)
{
	struct LinkNode* node = dealParamDec(ht, sz, p->firstChild);
	if (p->firstChild->nextSibling != NULL) {
		if (node != NULL)
			node->n_level = dealVarList(ht, sz, p->firstChild->nextSibling->nextSibling);
		else
			node = dealVarList(ht, sz, p->firstChild->nextSibling->nextSibling);
	}
	return node;
}

struct LinkNode* dealParamDec(struct HashEntry ht[], int sz, struct TNode *p)
{
	//Specifier
	struct Type* specifier = dealSpecifier(ht, sz, p->firstChild);	
	struct LinkNode* node = dealVarDec(ht, sz, p->firstChild->nextSibling, specifier, 0);
	if (node == NULL) return NULL;
	node->state = FuncParam;
	return node;
}

void dealCompSt(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s)
{
//	printf("dealStmtList~~~\n");
	dealDefList(ht, sz, p->firstChild->nextSibling, 0);
	dealStmtList(ht, sz, p->firstChild->nextSibling->nextSibling, s);
}

void dealStmtList(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s)
{
//	printf("dealStmtList~~~%s\n", p->firstChild->name);
	if (p->firstChild != NULL && strcmp(p->firstChild->name, "Stmt") == 0) {
		dealStmt(ht, sz, p->firstChild, s);
		dealStmtList(ht, sz, p->firstChild->nextSibling, s);
	}
}

void dealStmt(struct HashEntry ht[], int sz, struct TNode *p, struct Type* s)
{
//	printf("dealStmt~~~%s\n", p->firstChild->name);
	if (strcmp(p->firstChild->name, "Exp") == 0)		//Exp
		dealExp(ht, sz, p->firstChild);
	else if (strcmp(p->firstChild->name, "CompSt") == 0)	//{}
		dealCompSt(ht, sz, p->firstChild, s);
	else if (strcmp(p->firstChild->name, "RETURN") == 0) {	//RETURN Exp SEMI
		struct LinkNode *node = dealExp(ht, sz, p->firstChild->nextSibling);
		struct LinkNode *node1 = (struct LinkNode*)malloc(sizeof(struct LinkNode));
		node1->state = Func;
		node1->type = s;
		if (node != NULL && is_typeMatch(node1, node, NULL, p->firstChild->nextSibling) == 0) {
			printf("Error type 8 at line %d: Argument RETURN type mismatched.\n", p->line);
		}
	}
	else if (strcmp(p->firstChild->name, "IF") == 0) {	//if
		dealExp(ht, sz, p->firstChild->nextSibling->nextSibling);
//		printf("%s\n", p->firstChild->nextSibling->nextSibling->name);
		dealStmt(ht, sz, p->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, s);		
		if (p->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL)
			dealStmt(ht, sz, p->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling, s);
	}
	else if (strcmp(p->firstChild->name, "WHILE") == 0) {	//while
		dealExp(ht, sz, p->firstChild->nextSibling->nextSibling);
		dealStmt(ht, sz, p->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, s);
	}				
}

int dNum(struct LinkNode *n, struct TNode *p)
{
	int w1 = 0;
	struct Type* t = n->type;
	while(t->kind == array) {
		t = t->u.array.elem;
		w1++;
	}
	int w2 = 0;
	struct TNode *LB = p;
	while (LB->nextSibling != NULL && strcmp(LB->nextSibling->name, "LB") == 0) {
		LB = LB->firstChild;
		w2++;
	}	
	return w1-w2;
}

struct LinkNode* dealExp(struct HashEntry ht[], int sz, struct TNode *p)
{
//	printf("dealExp~~~%s\n", p->firstChild->name);
	if (strcmp(p->firstChild->name, "Exp") == 0) {
		struct TNode *p1 = p->firstChild;
		struct TNode *p2 = p->firstChild->nextSibling->nextSibling;
		struct LinkNode *n1 = dealExp(ht, sz, p1);
		if (n1 == NULL) return NULL;
		struct LinkNode *n2 = NULL;

		if (strcmp(p1->nextSibling->name, "DOT") == 0) {//struct
			if (n1->state == Struct || n1->type->kind != structure) {
				printf("Error type 13 at line %d: \"%s\" is not a Struct Variable\n", p1->line, n1->name);
				return NULL;
			}
			n2 = n1->type->u.structure->type->u.structure;
			while(n2 != NULL) {
//					printf("n2 %s, p2 %s\n", n2->name, p2->str);
				if (strcmp(n2->name, p2->str) == 0) 
					break;
				n2 = n2->n_level;
			}			
			if (n2 == NULL || n2->state != StructField) {
				printf("Error type 14 at line %d: Undefined variable \"%s\" in struct\n", p1->line, p2->str);
				return NULL;
			}
			return n2;
		}
		else {
			n2 = dealExp(ht, sz, p2);
		}

		if (strcmp(p1->nextSibling->name, "LB") == 0) {	//array
			if (n2 == NULL) return NULL;
//			printf("ARRAY\n");
			if (n1->type->kind != array) {
				printf("Error type 10 at line %d: Variable \"%s\" is not an Array\n", p1->line, n1->name);
				return NULL;
			}
			if (!(n2->type->kind == basic  && n2->type->u.basic == Integer)) {
				printf("Error type 12 at line %d: Array \"%s\" visit [not a Integer]\n", p1->line, n1->name);
				return NULL;
			}
			return n1;	
		}

		if (strcmp(p1->nextSibling->name, "ASSIGNOP") == 0) {	//=
//			printf("AASIGNOP\n");
			if (n2 == NULL) return n1;
			if ((strcmp(p1->firstChild->name, "ID") == 0 && p1->firstChild->nextSibling == NULL) 	//ID
				|| (p1->firstChild->nextSibling != NULL && (strcmp(p1->firstChild->nextSibling->name, "LB") == 0)) //array[]
				|| (p1->firstChild->nextSibling != NULL && (strcmp(p1->firstChild->nextSibling->name, "DOT") == 0))) {//struct. 
				//FuncParam, Variable 局部变量实现后别的函数的参数不会影响 
				while (p2->firstChild->nextSibling != NULL 
						&& (strcmp(p2->firstChild->nextSibling->name, "PLUS") == 0 
						|| strcmp(p2->firstChild->nextSibling->name, "MINUS") == 0 
						|| strcmp(p2->firstChild->nextSibling->name, "STAR") == 0 
						|| strcmp(p2->firstChild->nextSibling->name, "DIV") == 0)
						|| strcmp(p2->firstChild->name, "MINUS") == 0 
						|| strcmp(p2->firstChild->name, "LP") == 0) {	//Algorithm Exp
//					printf("p2->fc->ns %s\n", p2->firstChild->nextSibling->name);
					if (strcmp(p2->firstChild->name, "MINUS") == 0 || strcmp(p2->firstChild->name, "LP") == 0)
						p2 = p2->firstChild->nextSibling;
					else 
						p2 = p2->firstChild;
				}
				if (is_typeMatch(n1, n2, p1, p2) == 0) {// = ERROR TYPE 5!!!
					printf("Error type 5 at line %d: Type mismatched (=)\n", p->line);
					return NULL;
				}
			}
			else {
				printf("Error type 6 at line %d: ?? =\n", p->line);
				return NULL;
			}		
			return n1;
		}
		else if (strcmp(p1->nextSibling->name, "RELOP") == 0 
			|| strcmp(p1->nextSibling->name, "PLUS") == 0
			|| strcmp(p1->nextSibling->name, "MINUS") == 0
			|| strcmp(p1->nextSibling->name, "STAR") == 0
			|| strcmp(p1->nextSibling->name, "DIV") == 0) {	//relop, +,-,*,/
//			printf("Algorithm or logical exp\n");
			if (n2 == NULL) return NULL;
			if (n1->type->kind == structure) {
				printf("Error type 7 at line %d: Type of Operations mismatched (relop, +, -, *, /)\n", p->line);
				return NULL;
			}
			if (n2->type->kind == structure) {
				printf("Error type 7 at line %d: Type of Operations mismatched (relop, +, -, *, /)\n", p->line);
				return NULL;
			}
			while (p1->firstChild->nextSibling != NULL 
					&& (strcmp(p1->firstChild->nextSibling->name, "PLUS") == 0 
					|| strcmp(p1->firstChild->nextSibling->name, "MINUS") == 0 
					|| strcmp(p1->firstChild->nextSibling->name, "STAR") == 0 
					|| strcmp(p1->firstChild->nextSibling->name, "DIV") == 0)
					|| strcmp(p1->firstChild->name, "MINUS") == 0 
					|| strcmp(p1->firstChild->name, "LP") == 0) {	//Algorithm Exp
//				printf("p1->fc->ns %s\n", p1->firstChild->nextSibling->name);
				if (strcmp(p1->firstChild->name, "MINUS") == 0 || strcmp(p1->firstChild->name, "LP") == 0)
					p1 = p1->firstChild->nextSibling;
				else
					p1 = p1->firstChild;
			}
			while (p2->firstChild->nextSibling != NULL 
					&& (strcmp(p2->firstChild->nextSibling->name, "PLUS") == 0 
					|| strcmp(p2->firstChild->nextSibling->name, "MINUS") == 0 
					|| strcmp(p2->firstChild->nextSibling->name, "STAR") == 0 
					|| strcmp(p2->firstChild->nextSibling->name, "DIV") == 0)
					|| strcmp(p2->firstChild->name, "MINUS") == 0 
					|| strcmp(p2->firstChild->name, "LP") == 0) {	//Algorithm Exp
//				printf("p2->fc->ns %s\n", p2->firstChild->nextSibling->name);
				if (strcmp(p2->firstChild->name, "MINUS") == 0 || strcmp(p2->firstChild->name, "LP") == 0)
					p2 = p2->firstChild->nextSibling;
				else
					p2 = p2->firstChild;
			}
			while (strcmp(p1->firstChild->name, "MINUS") == 0 || strcmp(p1->firstChild->name, "LP") == 0)
				p1 = p1->firstChild->nextSibling;
			while (strcmp(p2->firstChild->name, "MINUS") == 0 || strcmp(p2->firstChild->name, "LP") == 0)
				p2 = p2->firstChild->nextSibling;
			if (is_typeMatch(n1, n2, p1, p2) == 0) {// = ERROR TYPE 5!!!
				printf("Error type 7 at line %d: Type of Operations mismatched (relop, +, -, *, /)\n", p->line);
				return NULL;
			}	
		}
		return n1;
	}
	else if (p->firstChild->nextSibling != NULL && strcmp(p->firstChild->nextSibling->name, "Exp") == 0) {//(x),-x,!x
		return dealExp(ht, sz, p->firstChild->nextSibling);
	}
	else if (strcmp(p->firstChild->name, "ID") == 0) {
		//ID查表...?
		struct LinkNode *node = NULL;
		if (p->firstChild->nextSibling == NULL) {//ID :no struct, structField, func; can be variable, FuncParam
			node = HashTableSearch(ht, sz, p->firstChild->str);
			if (node == NULL || node->state == Struct || node->state == StructField || node->state == Func) {
				printf("Error type 1 at line %d: Undefined variable \"%s\"\n", p->firstChild->line, p->firstChild->str);
				return NULL;
			}
		}
		else {					//ID LP Args RP, ID LP RP = func
			node = HashTableSearch(ht, sz, p->firstChild->str);
			if (node == NULL) {
				printf("Error type 2 at line %d: Undefined function \"%s\"\n", p->firstChild->line, p->firstChild->str); 
				return NULL;
			}
			if (node->state != Func) {//not function
				printf("Error type 11 at line %d: variable \"%s\" is not a function\n", p->line, p->firstChild->str); 
				return NULL;
			}
			if ((strcmp(p->firstChild->nextSibling->nextSibling->name, "RP") == 0 && node->type->u.function.paraNum != 0)
				|| (strcmp(p->firstChild->nextSibling->nextSibling->name, "Args") == 0 && node->type->u.function.paraNum == 0)) {
				printf("Error type 9 at line %d: Parameters Number mismatched \n", p->line); 
				return NULL;
			}
			if (strcmp(p->firstChild->nextSibling->nextSibling->name, "Args") == 0) {
				if (dealArgs(ht, sz, p->firstChild->nextSibling->nextSibling, node->type->u.function.paras) == NULL) //调用函数参数有问题
					return NULL;
			}
		}
		return node;
	}
	else if (strcmp(p->firstChild->name, "INT") == 0) {
		struct LinkNode *node = (struct LinkNode*)malloc(sizeof(struct LinkNode));
		node->name = "int";
		node->type = (struct Type*)malloc(sizeof(struct Type));
		node->type->kind = basic;
		node->type->u.basic = Integer;
		return node;
	}
	else if (strcmp(p->firstChild->name, "FLOAT") == 0) {
		struct LinkNode *node = (struct LinkNode*)malloc(sizeof(struct LinkNode));
		node->name = "float";
		node->type = (struct Type*)malloc(sizeof(struct Type));
		node->type->kind = basic;
		node->type->u.basic = Float;
		return node;
	}
	return NULL;
}

struct LinkNode* dealArgs(struct HashEntry ht[], int sz, struct TNode *p, struct LinkNode *n)
{
//	printf("dealArgs~~~\n");
	struct LinkNode* node = dealExp(ht, sz, p->firstChild);
	if (node == NULL) return NULL;
	if (is_typeMatch(node, n, p->firstChild, NULL) == 0) {
		printf("Error type 9 at line %d: Parameters Type mismatched \n", p->line);
		return NULL;
	}
	if ((p->firstChild->nextSibling == NULL && n->n_level != NULL) 
		|| (p->firstChild->nextSibling != NULL && n->n_level == NULL)) {
		printf("Error type 9 at line %d: Parameters Number mismatched \n", p->line);
		return NULL;
	}
	if (p->firstChild->nextSibling != NULL) 
		node = dealArgs(ht, sz, p->firstChild->nextSibling->nextSibling, n->n_level);
	return node;
}

int is_typeMatch(struct LinkNode* n1, struct LinkNode* n2, struct TNode *p1, struct TNode *p2) 	//match return 1, else return 0;
{
//	printf("is_typeMatch\n");
	struct Type *t1 = n1->type;
	struct Type *t2 = n2->type;
	if (t1->kind == function) 			//function retType
		t1 = t1->u.function.retType;
	if (t2->kind == function) 
		t2 = t2->u.function.retType;
	if (t1->kind == array) {			//array[]
		if (dNum(n1, p1->firstChild) != 0)
			return 0;
		while(t1->kind == array)
			t1 = t1->u.array.elem;
	}
	if (t2->kind == array) {
		if (dNum(n2, p2->firstChild) != 0)
			return 0;
		while(t2->kind == array)
			t2 = t2->u.array.elem;
	}

	if (t1->kind != t2->kind) 
		return 0;
	else {
		if (t1->kind == structure && strcmp(t2->u.structure->name, t1->u.structure->name) != 0) 
			return 0;
		else if (t1->kind == basic && t1->u.basic != t2->u.basic)
			return 0;
	}
	return 1;
}


