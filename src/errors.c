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


/*
 * Top Level Routines
 */


/*
 * Record an error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * params:
 *   ErrorCode   new_error   the error code to set
 */
void errors_set_code (ErrorCode new_error) {
  error = new_error;
}

/*
 * Return the last error encountered
 * globals:
 *   ErrorCode   error       the last error encountered
 * returns:
 *   ErrorCode               the last error encountered
 */
ErrorCode errors_get_code (void) {
  return error;
}