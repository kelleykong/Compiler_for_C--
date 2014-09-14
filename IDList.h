#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "Tree.h"

enum KindOfStatus {Empty, Program, Progress, Func, Struct, Variable, Const, Enum, ID, Array, StructField, FuncParam};
enum Vtype{Integer, Float};


//int Hsize = 16384;

struct HashEntry
{
	int linkNum;			//Link node number
	struct LinkNode *head;		//head pointer
};

struct Type
{
	enum {basic, array, structure, function } kind;
	union
	{
		enum Vtype basic;	//basic type
		struct {struct Type* elem; int size; } array;	//array:elem type, size
		struct LinkNode* structure;
		struct {struct Type* retType; int paraNum; struct LinkNode* paras; } function;
	} u;
};

struct LinkNode
{
	char* name;			//ID name
	enum KindOfStatus state;	//ID type:variable, struct, function, structField, FuncPara
	struct Operand *op;		//variable operand

	struct Type *type;		//basic, array, struct, function

	struct LinkNode *n_level;
	struct LinkNode *next;

//	LinkNode(): state(Empty) {}
};

struct LinkNode* HashTableInsert(struct HashEntry ht[], int sz, char* e);
struct LinkNode* HashTableSearch(struct HashEntry ht[], int sz, char* e);
int HashTableDelete(struct HashEntry ht[], int sz, char *e);
unsigned int hash_pjw(char* name);




