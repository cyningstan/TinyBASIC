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
  ProgramNode *program;
  LanguageOptions options = {
    LINE_NUMBERS_IMPLIED,
    255
  }; /* language options */
  ErrorCode code; /* error returned */
  StatementNode *statement; /* statement to execute/display */
  char *text; /* statement text to output */

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

  /* get the parse tree */
  options_set (options);
  program = parse_program (input);

  /* deal with errors */
  if ((code = errors_get_code ())) {
    printf ("Error code: %d\n", code);
    exit (code);
  }

  /* run/output the program */
  statement = program->first;
  while (statement) {
    text = statement_output (statement);
    printf ("%s", text);
    free (text);
    statement = statement->next;
  }

  /* return success */
  return 0;

}
