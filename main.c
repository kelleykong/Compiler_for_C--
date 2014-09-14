#include<stdio.h>
#include "IDList.h"
#include "ICGen.h"

extern FILE* yyin;
extern struct TNode* root;
struct HashEntry HashTable[16384];
extern struct ICNode* IChead;

int main(int argc, char** argv)
{
	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	initHashTable(HashTable, 16384);
//	printf("~~~~~HashTable completed!!!~~~~\n");
	createIDList(HashTable, 16384, root);
//	printf("~~~~~synmatic completed!!!~~~~~\n");
	translate(HashTable, root);
//	printICTable(IChead, argv[2]);
	genTargetCode(IChead, argv[2]);
	return 0;
}
