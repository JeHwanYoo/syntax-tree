#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lrparser.h"

const int action[NUM_SYM][NUM_TK] = {
	{5, 0, 0, 4, 0, 0}, {0, 6, 0, 0, 0, ACC}, {0, -2, 7, 0, -2, -2},
	{0, -4, -4, 0, -4, -4}, {5, 0, 0, 4, 0, 0}, {0, -6, -6, 0, -6, -6},
	{5, 0, 0, 4, 0, 0}, {5, 0, 0, 4, 0, 0}, {0, 6, 0, 0, 11, 0},
	{0, -1, 7, 0, -1, -1}, {0, -3, -3, 0, -3, -3}, {0, -5, -5, 0, -5, -5}
};

const int go_to[NUM_SYM][NUM_RULE] = {
	{1, 2, 3}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {8, 2, 3}, {0, 0, 0},
	{0, 9, 3}, {0, 0, 10}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

const int prod_left[NUM_TK+1] = {0, EXPRESSION, EXPRESSION, TERM, TERM, FACTOR, FACTOR};
const int prod_length[NUM_TK+1] = {0, 3, 1, 3, 1, 3, 1};

int stack[ST_SIZE];
NODE *value[ST_SIZE];
char yybuffer[ST_SIZE];
int pos = -1; // input position
int top = -1;
int sym;
int yyvalue;
int yytype;
static char ch = ' ';

char pos_getchar() {
	pos++;
	return getchar();
}

void push(int i) {
	stack[++top] = i;
}

void shift(int i) {
	push(i);
	if (sym == NUMBER) {
		value[top] = makenode(INT_VAL, yyvalue, NULL, NULL);
	}
	sym = yylex();
}

void reduce(int i) {
	int old_top;
	char buf[30];
	top -= prod_length[i];
	old_top = top;
	push(go_to[stack[old_top]][prod_left[i]]);
	switch(i) {
		case 1: value[top] = makenode(ADD, 0, value[old_top+1], value[old_top+3]);
						break;
		case 2: value[top] = value[old_top+1];
						break;
		case 3: value[top] = makenode(MUL, 0, value[old_top+1], value[old_top+3]);
						break;
		case 4: value[top] = value[old_top+1];
						break;
		case 5: value[top] = value[old_top+2];
						break;
		case 6: value[top] = makenode(INT_VAL, value[old_top+1]->val, NULL, NULL);
						break;
		default: sprintf(buf, "Reduce(%d) Error", i);
						 yyerr(buf);
						 break;
	}
}

void yyerr(char *msg) {
	printf("[Syntax Error] %s near position %d\n", msg, pos);
	exit(1);
}

void lexerr(char *msg) {
	printf("[Illegal Token] %s near position %d\n", msg, pos);
	exit(1);
}

void yywarning(char *msg) {
	printf("[Warning] %s near position %d\n", msg, pos);
}

void yyparse() {
	int i;
	NODE *ptr = NULL;
	stack[++top] = 0;
	sym = yylex();

	do {
		i = action[stack[top]][sym-TK_START];
		if (i == ACC) {
			printf("Success!\n");
		}
		else if (i > 0) {
			shift(i);
		}
		else if (i < 0) {
			reduce(-i);
		}
		else {
			yyerr("Action Parsing Table Error");
		}
	} while (i != ACC);

	ptr = value[top]; // root node
	print_tree_postorder(ptr); // traversal with postorder
	free_tree(ptr);
}

int yylex() {
	static char ch = ' ';
	int i = 0;
	while (ch == ' ' || ch == '\t' || ch == '\n') ch = pos_getchar();
	if (isdigit(ch)) {
		do {
			yybuffer[i++] = ch;
			ch = pos_getchar();
		} while (isdigit(ch));
		yybuffer[i]=0;
		yyvalue = atoi(yybuffer);
		memset(yybuffer, 0, ST_SIZE);
		return NUMBER;
	}
	else if (ch == '+') {
		ch = pos_getchar();
		return PLUS;
	}
	else if (ch == '*') {
		ch = pos_getchar();
		return STAR;
	}
	else if (ch == '(') {
		ch = pos_getchar();
		return LPAREN;
	}
	else if (ch == ')') {
		ch = pos_getchar();
		return RPAREN;
	}
	else if (ch == '$') {
		return END;
	}
	else {
		lexerr("Unknown Token");
	}
}


NODE *makenode(NODE_NAME name, int v, NODE *left, NODE *right) {
	NODE *n;
	n = malloc(sizeof(NODE));
	n->name = name;
	n->val = v;
	n->left = left;
	n->right = right;
	return n;
}

void print_tree_postorder(NODE *n) {
	if (n != NULL) {
		print_tree_postorder(n->left);
		print_tree_postorder(n->right);
		puts("------------------------------");
		print_node_name_as_string(n->name, "CURRENT");
		printf("CURRENT VALUE: %d\n", n->val);
		if (n->left != NULL) {
			print_node_name_as_string(n->left->name, "LEFT NODE");
			printf("LEFT NODE VALUE: %d\n", n->left->val);
		}
		if (n->right != NULL) {
			print_node_name_as_string(n->right->name, "RIGHT NODE");
			printf("RIGHT NODE VALUE: %d\n", n->right->val);
		}
	}
}

void print_node_name_as_string(NODE_NAME name, char *label) {
	char name_to_string[10];
	if (name == INT_VAL) {
		strcpy(name_to_string, "INT_VAL");
	} else if (name == ADD) {
		strcpy(name_to_string, "ADD");
	} else {
		strcpy(name_to_string, "MUL");
	}
	printf("%s NODE_NAME: %s\n", label, name_to_string);
}

void free_tree(NODE *n) {
	if (n != NULL) {
		free_tree(n->left);
		free_tree(n->right);
		free(n);
	}
}
