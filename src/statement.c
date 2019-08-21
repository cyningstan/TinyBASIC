/*
 * Tiny BASIC
 * Statement Handling Module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 15-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "statement.h"

/* constants */
#define STATEMENT_LENGTH 80 /* typical statement length */


/*
 * Level 1 Functions
 */


/*
 * LET statement output
 * params:
 *   LetStatementNode*   letn   data for the LET statement
 * returns:
 *   char*                      the LET statement text
 */
char *statement_output_let (LetStatementNode *letn) {

  /* local variables */
  char
    *let_text = NULL, /* the LET text to be assembled */
    *expression_text = NULL; /* the text of the expression */

  /* assemble the expression */
  expression_text = expression_output (letn->expression);

  /* assemble the final LET text, if we have an expression */
  if (expression_text) {
    let_text = malloc (7 + strlen (expression_text));
    sprintf (let_text, "LET %c=%s", 'A' - 1 + letn->variable, expression_text);
    free (expression_text);
  }

  /* return it */
  return let_text;
}


/*
 * Top Level Functions
 */


/*
 * Statement constructor
 * returns:
 *   StatementNode*   the newly-created blank statement
 */
StatementNode *statement_create (void) {

  /* local variables */
  StatementNode *statement; /* the created statement */

  /* allocate memory and set defaults */
  statement = malloc (sizeof (StatementNode));
  statement->label = 0;
  statement->class = STATEMENT_NONE;
  statement->next = NULL;

  /* return the created statement */
  return statement;
}

/*
 * Statement destructor
 * params:
 *   StatementNode*   statement   the doomed statement
 */
void statement_destroy (StatementNode *statement) {
   free (statement);
}

/*
 * Statement output
 * params:
 *   StatementNode*   statement   the statement to output
 * returns:
 *   char*                        a string containing the statement line
 */
char *statement_output (StatementNode *statement) {

  /* local variables */
  char
    label_text [7], /* line label text */
    *output = NULL, /* the rest of the output */
    *line = NULL; /* the assembled line */

  /* initialise the line label */
  if (statement->label) {
    sprintf (label_text, "%5d ", statement->label);
  else
    strcpy (label_text, "      ");

  /* build the statement itself */
  switch (statement->class) {
  case STATEMENT_LET:
    output = statement_output_let (statement->statement.letn);
    break;
  case STATEMENT_IF:
  case STATEMENT_GOTO:
  case STATEMENT_GOSUB:
  case STATEMENT_RETURN:
  case STATEMENT_END:
  case STATEMENT_PRINT:
  case STATEMENT_INPUT:
    output = malloc (17);
    sprintf (output, "Statement type %d", statement->class);
    break;
  default:
    output = malloc(24);
    strcpy (output, "Unrecognised statement.");
  }

  /* combine the two */
  line = malloc (strlen (label_text) + strlen (output) + 2);
  sprintf (line, "%s %s\n", label_text, output);
  free (output);

  /* return the listing line */
  return line;

}