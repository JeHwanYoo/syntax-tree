#ifndef __LRPARSER_H
#define __LRPARSER_H

enum token {
	NUMBER = 256,
	PLUS,
	STAR,
	LPAREN,
	RPAREN,
	END,
};

enum symbol {
	EXPRESSION = 0,
	TERM,
	FACTOR,
};

typedef enum {INT_VAL, ADD, MUL} NODE_NAME;
typedef struct node {NODE_NAME name; int val; struct node *left; struct node *right;} NODE;

#define ACC 999
#define NUM_SYM 12  // the number of symbols
#define NUM_TK 6    // the number of tokens
#define NUM_RULE 3  // the number of rules
#define TK_START 256 // the start number of token
#define ST_SIZE 1000

extern const int action[NUM_SYM][NUM_TK];
extern const int go_to[NUM_SYM][NUM_RULE];
extern const int prod_left[NUM_TK + 1];
extern const int prod_length[NUM_TK + 1];

extern int stack[ST_SIZE];
extern NODE *value[ST_SIZE];
extern char yybuffer[ST_SIZE];
extern int pos; // input position
extern int top;
extern int sym;
extern int yyvalue;

char pos_getchar(); // pos++; return getchar();
void push(int);
void shift(int);
void reduce(int);
void yyerr(char*);
void lexerr(char*);
void yywarning(char*);
void yyparse();
int yylex();

NODE *makenode(NODE_NAME name, int v, NODE *left, NODE *right);
void print_tree_postorder(NODE *n);
void print_node_name_as_string(NODE_NAME name, char *label);
void free_tree(NODE *n);

#endif

