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
#include "parser.h"
#include "statement.h"
#include "interpret.h"


/* static variables */
static char *input_filename = NULL; /* name of the input file */
static enum { /* action to take with parsed program */
  OUTPUT_INTERPRET, /* interpret the program */
  OUTPUT_LST /* output a formatted listing */
} output = OUTPUT_INTERPRET;


/*
 * Level 2 Routines
 */


/*
 * Set line number option
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_line_numbers (char *option) {
  if (! strncmp ("optional", option, strlen (option)))
    options_set_line_numbers (LINE_NUMBERS_OPTIONAL);
  else if (! strncmp ("implied", option, strlen (option)))
    options_set_line_numbers (LINE_NUMBERS_IMPLIED);
  else if (! strncmp ("mandatory", option, strlen (option)))
    options_set_line_numbers (LINE_NUMBERS_MANDATORY);
  else
    errors_set_code (E_BAD_COMMAND_LINE);
}

/*
 * Set line number limit
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_line_limit (char *option) {
  int limit; /* the limit contained in the option */
  if (sscanf (option, "%d", &limit))
    options_set_line_limit (limit);
  else
    errors_set_code (E_BAD_COMMAND_LINE);
}

/*
 * Set comment option
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_comments (char *option) {
  if (! strncmp ("enabled", option, strlen (option)))
    options_set_comments (COMMENTS_ENABLED);
  else if (! strncmp ("disabled", option, strlen (option)))
    options_set_comments (COMMENTS_DISABLED);
  else
    errors_set_code (E_BAD_COMMAND_LINE);
}

/*
 * Set the output options
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_output (char *option) {
  if (! strcmp ("lst", option))
    output = OUTPUT_LST;
  else
    errors_set_code (E_BAD_COMMAND_LINE);
}


/*
 * Level 1 Routines
 */


/*
 * Process the command line options
 * params:
 *   int     argc   number of arguments on the command line
 *   char**  argv   the arguments
 */
void tinybasic_options (int argc, char **argv) {

  /* local variables */
  int argn; /* argument number count */

  /* loop through all parameters */
  for (argn = 1; argn < argc && ! errors_get_code (); ++argn) {

    /* scan for line number options */
    if (! strncmp (argv[argn], "-n", 2))
      tinybasic_option_line_numbers (&argv[argn][2]);
    else if (! strncmp (argv[argn], "--line-numbers=", 15))
      tinybasic_option_line_numbers (&argv[argn][15]);

    /* scan for line number limit */
    else if (! strncmp (argv[argn], "-N", 2))
      tinybasic_option_line_limit (&argv[argn][2]);
    else if (! strncmp (argv[argn], "--line-limit=", 13))
      tinybasic_option_line_limit (&argv[argn][13]);

    /* scan for comment option */
    else if (! strncmp (argv[argn], "-o", 2))
      tinybasic_option_comments (&argv[argn][2]);
    else if (! strncmp (argv[argn], "--comments=", 11))
      tinybasic_option_comments (&argv[argn][11]);

    /* scan for output option */
    else if (! strncmp (argv[argn], "-O", 2))
      tinybasic_option_output (&argv[argn][2]);
    else if (! strncmp (argv[argn], "--output=", 9))
      tinybasic_option_comments (&argv[argn][9]);

    /* accept filename */
    else if (! input_filename)
      input_filename = argv[argn];

    /* raise an error upon illegal option */
    else
      errors_set_code (E_BAD_COMMAND_LINE);
  }
}

/*
 * Output a formatted program listing
 * params:
 *   ProgramNode*   program   the program to output
 */
void tinybasic_output_lst (ProgramNode *program) {

  /* local variables */
  FILE *output; /* the output file */
  char *output_filename; /* the output filename */
  ProgramLineNode *program_line; /* a statement we're listing */
  char *text; /* the text of a statement we're listing */

  /* open the output file */
  output_filename = malloc (strlen (input_filename) + 5);
  sprintf (output_filename, "%s.lst", input_filename);
  if ((output = fopen (output_filename, "w"))) {

    /* write to the output file */
    program_line = program->first;
    while (program_line) {
      text = program_line_output (program_line);
      fprintf (output, "%s", text);
      free (text);
      program_line = program_line->next;
    }
    fclose (output);
  }

  /* deal with errors */
  else
    errors_set_code (E_FILE_NOT_FOUND);
}


/*
 * Top Level Routine
 */


/*
 * Main Program
 * params:
 *   int     argc   number of arguments on the command line
 *   char**  argv   the arguments
 * returns:
 *   int            any error code from processing/running the program
 */
int main (int argc, char **argv) {

  /* local variables */
  FILE *input; /* input file */
  ProgramNode *program; /* the parsed program */
  ErrorCode code; /* error returned */

  /* interpret the command line arguments */
  tinybasic_options (argc, argv);

  /* give usage if filename not given */
  if (! input_filename) {
    printf ("Usage: %s [OPTIONS] INPUT-FILE\n", argv [0]);
    return 0;
  }

  /* otherwise attempt to open the file */
  if (!(input = fopen (input_filename, "r"))) {
    printf ("Error: cannot open file %s\n", argv [1]);
    return E_FILE_NOT_FOUND;
  }

  /* get the parse tree */
  program = parse_program (input);

  /* deal with errors */
  if ((code = errors_get_code ())) {
    printf ("Parse error code: %d\n", code);
    exit (code);
  }

  /* perform the desired action */
  switch (output) {
    case OUTPUT_INTERPRET:
      interpret_program (program);
      if ((code = errors_get_code ()))
        printf ("Runtime error code: %d\n", code);
      break;
    case OUTPUT_LST:
      tinybasic_output_lst (program);
      break;
  }

  /* return success */
  return 0;

}
