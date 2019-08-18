/*
 * Tiny BASIC
 * Interpreter and Compiler Main Program
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 04-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "tokeniser.h"
#include "parser.h"
#include "statement.h"


/* test */
int main (int argc, char **argv) {

  FILE *input; /* input file */
  StatementNode *statement; /* statement loaded from file */
  LanguageOptions options = {
    LINE_NUMBERS_IMPLIED,
    255
  }; /* language options */

  /* give usage if argument not given */
  if (argc != 2) {
    printf ("Usage: %s INPUT-FILE\n", argv [0]);
    return 0;
  }

  /* otherwise attempt to open the file */
  if (!(input = fopen (argv [1], "r"))) {
    printf ("Error: cannot open file %s\n", argv [1]);
    return 1;
  }

  /* get first statement */
  set_language_options (options);
  while ((statement = get_next_statement (input)) && ! get_error ()) {
    printf ("Found a statement: %5d %d\n", statement->label, statement->class);
    statement_destroy (statement);
  }
  printf ("Error code: %d\n", get_error ());

  /* return success */
  return 0;

}
