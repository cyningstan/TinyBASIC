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
#include "listing.h"
#include "generatec.h"


/* static variables */
static char *input_filename = NULL; /* name of the input file */
static enum { /* action to take with parsed program */
  OUTPUT_INTERPRET, /* interpret the program */
  OUTPUT_LST, /* output a formatted listing */
  OUTPUT_C, /* output a C program */
  OUTPUT_EXE /* output an executable */
} output = OUTPUT_INTERPRET;
static ErrorHandler *errors; /* universal error handler */
static LanguageOptions *loptions; /* language options */


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
    loptions->set_line_numbers (loptions, LINE_NUMBERS_OPTIONAL);
  else if (! strncmp ("implied", option, strlen (option)))
    loptions->set_line_numbers (loptions, LINE_NUMBERS_IMPLIED);
  else if (! strncmp ("mandatory", option, strlen (option)))
    loptions->set_line_numbers (loptions, LINE_NUMBERS_MANDATORY);
  else
    errors->set_code (errors, E_BAD_COMMAND_LINE, 0, 0);
}

/*
 * Set line number limit
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_line_limit (char *option) {
  int limit; /* the limit contained in the option */
  if (sscanf (option, "%d", &limit))
    loptions->set_line_limit (loptions, limit);
  else
    errors->set_code (errors, E_BAD_COMMAND_LINE, 0, 0);
}

/*
 * Set comment option
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_comments (char *option) {
  if (! strncmp ("enabled", option, strlen (option)))
    loptions->set_comments (loptions, COMMENTS_ENABLED);
  else if (! strncmp ("disabled", option, strlen (option)))
    loptions->set_comments (loptions, COMMENTS_DISABLED);
  else
    errors->set_code (errors, E_BAD_COMMAND_LINE, 0, 0);
}

/*
 * Set the output options
 * params:
 *   char*   option   the option supplied on the command line
 */
void tinybasic_option_output (char *option) {
  if (! strcmp ("lst", option))
    output = OUTPUT_LST;
  else if (! strcmp ("c", option))
    output = OUTPUT_C;
  else if (! strcmp ("exe", option))
    output = OUTPUT_EXE;
  else
    errors->set_code (errors, E_BAD_COMMAND_LINE, 0, 0);
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
  for (argn = 1; argn < argc && ! errors->get_code (errors); ++argn) {

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
      tinybasic_option_output (&argv[argn][9]);

    /* accept filename */
    else if (! input_filename)
      input_filename = argv[argn];

    /* raise an error upon illegal option */
    else
      errors->set_code (errors, E_BAD_COMMAND_LINE, 0, 0);
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
      if ((text = listing_line_output (program_line, errors))) {
        fprintf (output, "%s", text);
        free (text);
      }
      program_line = program_line->next;
    }
    fclose (output);
  }

  /* deal with errors */
  else
    errors->set_code (errors, E_FILE_NOT_FOUND, 0, 0);
}

/*
 * Output a C source file
 * params:
 *   ProgramNode*   program   the parsed program
 */
void tinybasic_output_c (ProgramNode *program) {

  /* local variables */
  FILE *output; /* the output file */
  char *output_filename; /* the output filename */
  CProgram *c_program; /* the C program */

  /* open the output file */
  output_filename = malloc (strlen (input_filename) + 5);
  sprintf (output_filename, "%s.c", input_filename);
  if ((output = fopen (output_filename, "w"))) {

    /* write to the output file */
    c_program = new_CProgram (errors, loptions);
    if (c_program) {
      c_program->generate (c_program, program);
      if (c_program->c_output)
        fprintf (output, "%s", c_program->c_output);
      c_program->destroy (c_program);
    }
    fclose (output);
  }

  /* deal with errors */
  else
    errors->set_code (errors, E_FILE_NOT_FOUND, 0, 0);

  /* clean up allocated memory */
  free (output_filename);
}

/*
 * Invoke a compiler to turn a C source file into an executable
 * params:
 *   char*   basic_filename   The BASIC program's name
 */
void tinybasic_output_exe (char *command, char *basic_filename) {

  /* local variables */
  char
    c_filename[256], /* the name of the C source */
    exe_filename[256], /* the base name of the executable */
    final_command[1024], /* the constructed compiler command */
    *ext, /* position of extension character '.' in filename */
    *src, /* source pointer for string copying */
    *dst; /* destination pointer for string copying */

  /* work out the C and EXE filenames */
  sprintf (c_filename, "%s.c", basic_filename);
  strcpy (exe_filename, basic_filename);
  if ((ext = strchr (exe_filename, '.')))
    *ext = '\0';
  else
    strcat (exe_filename, ".out");

  /* build the compiler command */
  src = command;
  dst = final_command;
  while (*src) {
    if (! strncmp (src, "$(TARGET)", strlen ("$(TARGET)"))) {
      strcpy (dst, exe_filename);
      dst += strlen (exe_filename);
      src += strlen ("$(TARGET)");
    } else if (! strncmp (src, "$(SOURCE)", strlen ("$(SOURCE)"))) {
      strcpy (dst, c_filename);
      dst += strlen (c_filename);
      src += strlen ("$(SOURCE)");
    } else
      *(dst++) = *(src++);
  }
  *dst = '\0';

  /* run the compiler command */
  system (final_command);
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
  char
    *error_text, /* error text message */
    *command; /* command for compilation */

  /* interpret the command line arguments */
  errors = new_ErrorHandler ();
  loptions = new_LanguageOptions ();
  tinybasic_options (argc, argv);

  /* give usage if filename not given */
  if (! input_filename) {
    printf ("Usage: %s [OPTIONS] INPUT-FILE\n", argv [0]);
    errors->destroy (errors);
    loptions->destroy (loptions);
    return 0;
  }

  /* otherwise attempt to open the file */
  if (!(input = fopen (input_filename, "r"))) {
    printf ("Error: cannot open file %s\n", input_filename);
    errors->destroy (errors);
    loptions->destroy (loptions);
    return E_FILE_NOT_FOUND;
  }

  /* get the parse tree */
  program = parse_program (input, errors, loptions);
  fclose (input);

  /* deal with errors */
  if ((code = errors->get_code (errors))) {
    error_text = errors->get_text (errors);
    printf ("Parse error: %s\n", error_text);
    free (error_text);
    loptions->destroy (loptions);
    errors->destroy (errors);
    return code;
  }

  /* perform the desired action */
  switch (output) {
    case OUTPUT_INTERPRET:
      interpret_program (program, errors, loptions);
      if ((code = errors->get_code (errors))) {
        error_text = errors->get_text (errors);
        printf ("Runtime error: %s\n", error_text);
        free (error_text);
      }
      break;
    case OUTPUT_LST:
      tinybasic_output_lst (program);
      break;
    case OUTPUT_C:
      tinybasic_output_c (program);
      break;
    case OUTPUT_EXE:
      if ((command = getenv ("TBEXE"))) {
        tinybasic_output_c (program);
        tinybasic_output_exe (command, input_filename);
      } else
        printf ("TBEXE not set.\n");
      break;
  }

  /* clean up and return success */
  program_destroy (program);
  loptions->destroy (loptions);
  errors->destroy (errors);
  return 0;

}
