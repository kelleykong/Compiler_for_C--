#include<stdio.h>
#include<string.h>
enum type{empty, string, integer, floatt, relopp };
enum Relop{equal, notEqual, less, lessEqual, greater, greaterEqual };
struct TNode {
	char* name;
	int line;
	enum type tpe;
	int x;
	float f;
	char* str;
	enum Relop relop;
	struct TNode *firstChild, *nextSibling;
};


void printTree(struct TNode *r, int spcN);
void insert(struct TNode *p, struct TNode *c);
