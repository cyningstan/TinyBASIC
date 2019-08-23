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
static LanguageOptions options = { /* master language options */
  LINE_NUMBERS_IMPLIED,
  255,
  COMMENTS_ENABLED
};


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

/*
 * Set the line number option individually
 * globals:
 *   LanguageOptions    options        the options
 * params:
 *   LineNumberOption   line_numbers   line number option to set
 */
void options_set_line_numbers (LineNumberOption line_numbers) {
  options.line_numbers = line_numbers;
}

/*
 * Set the line number limit individually
 * globals:
 *   LanguageOptions    options        the options
 * params:
 *   int                line_limit     line number limit to set
 */
void options_set_line_limit (int line_limit) {
  options.line_limit = line_limit;
}

/*
 * Set the comments option individually
 * globals:
 *   LanguageOptions    options    the options
 * params:
 *   CommetOption       comments   comment option to set
 */
void options_set_comments (CommentOption comments) {
  options.comments = comments;
}

