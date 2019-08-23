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
  E_LAST /* placeholder */
} ErrorCode;


/*
 * Record an error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * params:
 *   ErrorCode   new_error   the error code to set
 */
void errors_set_code (ErrorCode new_error);

/*
 * Return the last error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * returns:
 *   ErrorCode               the last error encountered
 */
ErrorCode errors_get_code (void);


#endif