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
  E_LAST /* placeholder */
} ErrorCode;



#endif