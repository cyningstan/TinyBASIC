/*
 * Tiny BASIC
 * Error Handling Header
 *
 * Copyright (C) Damian Gareth Walker 2019
 * Created: 15-Aug-2019
 */


#ifndef __ERRORS_H__
#define __ERRORS_H__


/*
 * Data Definitions
 */

/* error codes */
typedef enum {
  E_NONE, /* no error; everything is fine */
  E_INVALID_LINE_NUMBER, /* line number is invalid */
  E_UNRECOGNISED_COMMAND, /* command was not recognised */
  E_INVALID_VARIABLE, /* variable expected but something else encountered */
  E_INVALID_ASSIGNMENT, /* = expected but something else encountered */
  E_INVALID_EXPRESSION, /* an invalid expression was encountered */
  E_MISSING_RIGHT_PARENTHESIS, /* Encountered "(" without corresponding ")" */
  E_INVALID_PRINT_OUTPUT, /* failed to parse print output */
  E_BAD_COMMAND_LINE, /* error on invocation */
  E_FILE_NOT_FOUND, /* cannot open source file */
  E_INVALID_OPERATOR, /* unrecognised operator */
  E_THEN_EXPECTED, /* didn't find the expected THEN after an IF */
  E_UNEXPECTED_PARAMETER, /* more parameters encountered than expected */
  E_RETURN_WITHOUT_GOSUB, /* return encountered without a GOSUB */
  E_DIVIDE_BY_ZERO, /* an attempt to divide by zero */
  E_LAST /* placeholder */
} ErrorCode;


/*
 * Record an error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * params:
 *   ErrorCode   new_error   the error code to set
 */
void errors_set_code (ErrorCode new_error, int line, int label);

/*
 * Return the last error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * returns:
 *   ErrorCode               the last error encountered
 *   int         line        the source line on which the error occurred
 *   int         label       the erroneous line's label
 */
ErrorCode errors_get_code (void);

/*
 * Return the last error line encountered
 * globals:
 *   int   line   the source line of the last error
 * returns:
 *   int          the source line to return
 */
ErrorCode errors_get_line (void);

/*
 * Return the last error label encountered
 * globals:
 *   int   label   the line label of the last error
 * returns:
 *   int           the line label to return
 */
ErrorCode errors_get_label (void);

/*
 * Generate an error message
 * globals:
 *   ErrorCode   error   the code is used to look up the message
 *   int         line    the source line is added for token/parse errors
 *   int         label   the line's label is added for parse/runtime errors
 * returns:
 *   char*               the full error message to return
 */
char *errors_text (void);

#endif