/*
 * Tiny BASIC
 * Parser Header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 08-Aug-2019
 */


#ifndef __PARSER_H__
#define __PARSER_H__


/* pre-requisite headers */
#include "statement.h"
#include "expression.h"
#include "errors.h"


/* Forward Declarations */
typedef struct block_node BlockNode;

/*
 * Non-BASIC structures and types
 */

/* line number options */
typedef enum {
  LINE_NUMBERS_OPTIONAL, /* they are optional numeric labels */
  LINE_NUMBERS_IMPLIED, /* missing line numbers are implied */
  LINE_NUMBERS_MANDATORY /* every line requires a number in sequence */
} LineNumberOption;

/* language options */
typedef struct {
  LineNumberOption line_numbers; /* mandatory, implied, optional */
  int line_limit; /* highest line number allowed */
} LanguageOptions;

/* Block Node */
typedef struct block_node {
  int label; /* the line number of the block */
  StatementNode *statement; /* the first statement */
  BlockNode *next; /* the next block */
} BlockNode;


/*
 * Function Declarations
 */


/* Destructor for an ExpressionNode */
void destroy_expression (ExpressionNode *expression);

/* set the language options */
void set_language_options (LanguageOptions input_options);

/* get a statement */
StatementNode *get_next_statement (FILE *input);

/* return the last error */
ErrorCode get_error (void);


#endif
