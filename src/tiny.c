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
#include "options.h"
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
  char *statement_text = NULL; /* the listing output for a statement */

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
  options_set (options);
  while ((statement = get_next_statement (input)) && ! errors_get_code ()) {
    if ((statement_text = statement_output (statement))) {
      printf ("%s", statement_text);
      free (statement_text);
    }
    statement_destroy (statement);
  }
  printf ("Error code: %d\n", errors_get_code ());

  /* return success */
  return 0;

}
