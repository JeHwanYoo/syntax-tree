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
V value[ST_SIZE];
char yybuffer[ST_SIZE];
int pos = -1; // input position
int top = -1;
int sym;
V yyvalue;
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
		value[top] = yyvalue;
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
		case 1: value[top] = calc(&value[old_top+1], &value[old_top+3], PLUS);
						break;
		case 2: value[top] = value[old_top+1];
						break;
		case 3: value[top] = calc(&value[old_top+1], &value[old_top+3], STAR);
						break;
		case 4: value[top] = value[old_top+1];
						break;
		case 5: value[top] = value[old_top+2];
						break;
		case 6: value[top] = value[old_top+1];
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

	if (value[1].t == INTEGER) {
		printf("Calculation Result: %d\n", value[1].v.i);
	}
	else {
		printf("Calculation Result: %lf\n", value[1].v.d);
	}
}

int yylex() {
	static char ch = ' ';
	int i = 0;
	int dot = 0;
	while (ch == ' ' || ch == '\t' || ch == '\n') ch = pos_getchar();
	if (isdigit(ch)) {
		do {
			if (ch == '.') {
				if (dot == 1) {
					lexerr("Wrong Number Format");
				} else {
					dot = DOUBLE;
				}
			}
			yybuffer[i++] = ch;
			ch = pos_getchar();
		} while (isdigit(ch) || ch == '.');
		yybuffer[i]=0;
		if (dot == DOUBLE) {
			yyvalue.v.d = atof(yybuffer);
			yyvalue.t = DOUBLE;
		} else {
			yyvalue.v.i = atof(yybuffer);
			yyvalue.t = INTEGER;
		}
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

V calc(V* a, V* b, int op) {
	V new_val;
	char buf[100];
	//	if type different
	if (a->t != b->t) {
		new_val.t = DOUBLE;
		if (a->t == INTEGER && b->t == DOUBLE) {
			sprintf(buf, "Caculation Between int(%d) and double(%lf)", a->v.i, b->v.d);
			yywarning(buf);
			if (op == PLUS) {
				new_val.v.d = ((double)a->v.i) + b->v.d;
			}
			else if (op == STAR) {
				new_val.v.d = ((double)a->v.i) * b->v.d;
			}
		}
		else if (a->t == DOUBLE && b->t == INTEGER) {
			sprintf(buf, "Caculation Between double(%lf) and int(%d)", a->v.d, b->v.i);
			yywarning(buf);
			if (op == PLUS) {
				new_val.v.d = a->v.d + ((double)b->v.i);
			}
			else if (op == STAR) {
				new_val.v.d = a->v.d * ((double)b->v.i);
			}
		}
	}
	else {
		if (a->t == INTEGER) {
			new_val.t = INTEGER;
			if (op == PLUS) {
				new_val.v.i = a->v.i + b->v.i;
			}
			else if (op == STAR) {
				new_val.v.i = a->v.i * b->v.i;
			}
		}
		else if (a->t == DOUBLE) {
			new_val.t = DOUBLE;
			if (op == PLUS) {
				new_val.v.d = a->v.d + b->v.d;
			}
			else if (op == STAR) {
				new_val.v.d = a->v.d * b->v.d;
			}
		}
	}
	return new_val;
}

