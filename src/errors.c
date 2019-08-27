/*
 * Tiny BASIC
 * Error Handling Module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 18-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

/* global variables */
static ErrorCode error = E_NONE; /* the last error encountered */
static int line = 0; /* the source line on which the error occurred */
static int label = 0; /* the label for the source line */
static char *messages[E_LAST] = { /* the error messages */
  "Successful",
  "Invalid line number",
  "Unrecognised command",
  "Invalid variable",
  "Invalid assignment",
  "Invalid expression",
  "Missing )",
  "Invalid PRINT output",
  "Bad command line",
  "File not found",
  "Invalid operator",
  "THEN expected",
  "Unexpected parameter",
  "RETURN without GOSUB",
  "Divide by zero"
};


/*
 * Top Level Routines
 */


/*
 * Record an error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 *   int         line        the source line
 *   int         label       the line's label
 * params:
 *   ErrorCode   new_error   the error code to set
 *   int         new_line    the source line to set
 *   int         new_label   the label to set
 */
void errors_set_code (ErrorCode new_error, int new_line, int new_label) {
  error = new_error;
  line = new_line;
  label = new_label;
}

/*
 * Return the last error code encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * returns:
 *   ErrorCode               the last error encountered
 */
ErrorCode errors_get_code (void) {
  return error;
}

/*
 * Return the last error line encountered
 * globals:
 *   int   line   the source line of the last error
 * returns:
 *   int          the source line to return
 */
ErrorCode errors_get_line (void) {
  return line;
}

/*
 * Return the last error label encountered
 * globals:
 *   int   label   the line label of the last error
 * returns:
 *   int           the line label to return
 */
ErrorCode errors_get_label (void) {
  return label;
}

/*
 * Generate an error message
 * globals:
 *   ErrorCode   error   the code is used to look up the message
 *   int         line    the source line is added for token/parse errors
 *   int         label   the line's label is added for parse/runtime errors
 * returns:
 *   char*               the full error message to return
 */
char *errors_text (void) {

  /* local variables */
  char
    *message, /* the complete message */
    *line_text, /* source line N */
    *label_text; /* label N */

  /* get the source line, if there is one */
  line_text = malloc (20);
  if (line)
    sprintf (line_text, ", source line %d", line);
  else
    strcpy (line_text, "");

  /* get the source label, if there is one */
  label_text = malloc (19);
  if (label)
    sprintf (label_text, ", line label %d", label);
  else
    strcpy (label_text, "");

  /* put the error message together */
  message = malloc (strlen (messages[error]) + strlen (line_text)
    + strlen (label_text) + 1);
  strcpy (message, messages[error]);
  strcat (message, line_text);
  strcat (message, label_text);
  free (line_text);
  free (label_text);

  /* return the assembled error message */
  return message;
}