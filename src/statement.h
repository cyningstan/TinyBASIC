/*
 * Tiny BASIC
 * Statement Handling Header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 15-Aug-2019
 */


#ifndef __STATEMENT_H__
#define __STATEMENT_H__


/* Pre-requisite headers */
#include "expression.h"

/* Forward Declarations */
typedef struct statement_node StatementNode;
typedef struct output_node OutputNode;
typedef struct variable_list_node VariableListNode;


/*
 * Data Definitions
 */


/* The types of output allowed in a PRINT statement */
typedef enum {
  OUTPUT_STRING, /* a literal string */
  OUTPUT_EXPRESSION /* an expression */
} OutputClass;

/* The relational operators */
typedef enum {
  RELOP_EQUAL, /* = */
  RELOP_UNEQUAL, /* <> or >< */
  RELOP_LESSTHAN, /* < */
  RELOP_LESSOREQUAL, /* <= */
  RELOP_GREATERTHAN, /* > */
  RELOP_GREATEROREQUAL /* >= */
} RelationalOperator;

/* Expressions or strings to output */
typedef struct output_node {
  OutputClass class; /* string or expression */
  union {
    char *string; /* a literal string to output */
    ExpressionNode *expression; /* an expression to output */
  } output;
  OutputNode *next; /* the next output node, if any */
} OutputNode;

/* List of variables to input */
typedef struct variable_list_node {
  int variable; /* the variable */
  VariableListNode *next; /* next variable, if any */
} VariableListNode;

/*
 * Structures for statements in general
 */

/* Let Statement Node */
typedef struct {
  int variable; /* the variable to assign, 1..26 for A..Z */
  ExpressionNode *expression; /* the expression to assign to it */
} LetStatementNode;

/* If Statement Node */
typedef struct {
  ExpressionNode *left; /* the left-hand expression */
  RelationalOperator op; /* the comparison operator used */
  ExpressionNode *right; /* the right-hand expression */
  StatementNode *statement; /* statement to execute if condition is true */
} IfStatementNode;

/* Print Statement Node */
typedef struct {
  OutputNode *first; /* the first expression to print */
} PrintStatementNode;

/* Input Statement Node */
typedef struct {
  VariableListNode *first; /* the first variable to input */
} InputStatementNode;

/* Goto Statement Node */
typedef struct {
  ExpressionNode *label; /* an expression that computes the label */
} GotoStatementNode;

/* Gosub Statement Node */
typedef struct {
  ExpressionNode *label; /* an expression that computes the label */
} GosubStatementNode;

/* Statement classes */
typedef enum {
  STATEMENT_NONE, /* unknown statement */
  STATEMENT_LET, /* LET variable=expression */
  STATEMENT_IF, /* IF condition THEN statement */
  STATEMENT_GOTO, /* GOTO expression */
  STATEMENT_GOSUB, /* GOSUB expression */
  STATEMENT_RETURN, /* RETURN */
  STATEMENT_END, /* END */
  STATEMENT_PRINT, /* PRINT print-list */
  STATEMENT_INPUT /* INPUT var-list */
} StatementClass;

/* Common Statement Node */
typedef struct statement_node {
  int label; /* line label */
  StatementClass class; /* which type of statement this is */
  union {
    LetStatementNode *letn; /* a LET statement */
    IfStatementNode *ifn; /* an IF statement */
    GotoStatementNode *goton; /* a GOTO statement */
    GosubStatementNode *gosubn; /* a GOSUB statement */
    /* a RETURN statement requires no extra data */
    /* an END statement requires no extra data */
    PrintStatementNode *printn; /* a PRINT statement */
    InputStatementNode *inputn; /* an INPUT statement */
  } statement;
  StatementNode *next; /* the next statement */
} StatementNode;


/*
 * Function Declarations
 */


/*
 * Statement constructor
 * returns:
 *   StatementNode*   the newly-created blank statement
 */
StatementNode *statement_create (void);

/*
 * Statement destructor
 * params:
 *   StatementNode*   statement   the doomed statement
 */
void statement_destroy (StatementNode *statement);

/*
 * Statement output
 * params:
 *   StatementNode*   statement   the statement to output
 * returns:
 *   char*                        a string containing the statement line
 */
char *statement_output (StatementNode *statement);


#endif