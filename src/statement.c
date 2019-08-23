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


/*
 * Level 1 Functions
 */


/*
 * LET statement constructor
 * returns:
 *   LetStatementNode*   the created LET statement
 */
LetStatementNode *statement_create_let (void) {

  /* local variables */
  LetStatementNode *letn; /* the created node */

  /* allocate memory and assign safe defaults */
  letn = malloc (sizeof (LetStatementNode));
  letn->variable = 0;
  letn->expression = NULL;

  /* return the LET statement node */
  return letn;
}

/*
 * Destructor for a LET statement
 * params:
 *   LetStatementNode   *letn   the doomed LET statement.
 */
void statement_destroy_let (LetStatementNode *letn) {
  if (letn->expression)
    expression_destroy (letn->expression);
  free (letn);
}

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
 * PRINT statement constructor
 * returns:
 *   PrintStatementNode*   the created PRINT statement
 */
PrintStatementNode *statement_create_print (void) {

  /* local variables */
  PrintStatementNode *printn; /* the created node */

  /* allocate memory and assign safe defaults */
  printn = malloc (sizeof (PrintStatementNode));
  printn->first = NULL;

  /* return the PRINT statement node */
  return printn;
}

/*
 * Destructor for a PRINT statement
 * params:
 *   PrintStatementNode   *printn   the doomed PRINT statement.
 */
void statement_destroy_print (PrintStatementNode *printn) {
  OutputNode *current, *next;
  current = printn->first;
  while (current) {
    next = current->next;
    free (current);
    current = next;
  }
  free (printn);
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
  switch (statement->class) {
    case STATEMENT_LET:
      statement_destroy_let (statement->statement.letn);
      break;
    case STATEMENT_PRINT:
      statement_destroy_print (statement->statement.printn);
      break;
    default:
      break;
  }
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
  if (statement->label)
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
    output = malloc (24);
    strcpy (output, "Unrecognised statement.");
  }

  /* combine the two */
  line = malloc (strlen (label_text) + strlen (output) + 2);
  sprintf (line, "%s%s\n", label_text, output);
  free (output);

  /* return the listing line */
  return line;

}