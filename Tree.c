#include "Tree.h"

void printTree(struct TNode *r, int spcN) {
	int i;
	if (strcmp(r->name, "Empty") != 0) {
		for(i = 0; i < spcN; i++)
			printf("  ");
		if (r->firstChild == NULL) {
			if (r->tpe == integer)
				printf("%s: %d\n", r->name, r->x);
			else if (r->tpe == floatt)
				printf("%s: %f\n", r->name, r->f);
			else if (r->tpe == string)
				printf("%s: %s\n", r->name, r->str);
			else
				printf("%s\n", r->name);
		}
		else
			printf("%s (%d)\n", r->name, r->line);
	}
	if (r->firstChild != NULL)
		printTree(r->firstChild, spcN+1);
	if (r->nextSibling != NULL)
		printTree(r->nextSibling, spcN);
}
	
void insert(struct TNode *p, struct TNode *c) {
//	printf("insert %s as %s child\n", c->name, p->name);
	if(p->firstChild == NULL) 
		p->firstChild = c;
	else {
		p = p->firstChild;
		while(p->nextSibling != NULL) {
			p = p->nextSibling;
		}
		p->nextSibling = c;
	}
}
