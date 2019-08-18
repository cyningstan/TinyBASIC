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