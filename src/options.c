/*
 * Tiny BASIC Interpreter and Compiler Project
 * Language Options Module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 18-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "options.h"

/* global variables */
static LanguageOptions options; /* master language options */


/*
 * Top Level Functions
 */


/*
 * Set the language options as a group
 * globals:
 *   LanguageOptions   options       the master language options
 * params:
 *   LanguageOptions   new_options   the options to set
 */
void options_set (LanguageOptions new_options) {
  options = new_options;
}


/*
 * Get the language options as a group
 * globals:
 *   LanguageOptions   options       the master language options
 * params:
 *   LanguageOptions                 the options currently set
 */
LanguageOptions options_get (void) {
  return options;
}