/*
 * Tiny BASIC Interpreter and Compiler Project
 * Language Options Header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 18-Aug-2019
 */


#ifndef __OPTIONS_H__
#define __OPTIONS_H__


/*
 * Data Definitions
 */


/* line number options */
typedef enum {
  LINE_NUMBERS_OPTIONAL, /* they are optional numeric labels */
  LINE_NUMBERS_IMPLIED, /* missing line numbers are implied */
  LINE_NUMBERS_MANDATORY /* every line requires a number in sequence */
} LineNumberOption;

/* comment options */
typedef enum {
  COMMENTS_ENABLED, /* comments and blank lines are allowed */
  COMMENTS_DISABLED /* comments and blank lines are not allowed */
} CommentOption;

/* language options */
typedef struct {
  LineNumberOption line_numbers; /* mandatory, implied, optional */
  int line_limit; /* highest line number allowed */
  CommentOption comments; /* enabled, disabled */
} LanguageOptions;


/*
 * Function Declarations
 */


/*
 * Set the language options as a group
 * globals:
 *   LanguageOptions   options       the master language options
 * params:
 *   LanguageOptions   new_options   the options to set
 */
void options_set (LanguageOptions new_options);

/*
 * Get the language options as a group
 * globals:
 *   LanguageOptions   options       the master language options
 * params:
 *   LanguageOptions                 the options currently set
 */
LanguageOptions options_get (void);

/*
 * Set the line number option individually
 * globals:
 *   LanguageOptions    options        the options
 * params:
 *   LineNumberOption   line_numbers   line number option to set
 */
void options_set_line_numbers (LineNumberOption line_numbers);

/*
 * Set the line number limit individually
 * globals:
 *   LanguageOptions    options        the options
 * params:
 *   int                line_limit     line number limit to set
 */
void options_set_line_limit (int line_limit);

/*
 * Set the comments option individually
 * globals:
 *   LanguageOptions    options    the options
 * params:
 *   CommetOption       comments   comment option to set
 */
void options_set_comments (CommentOption comments);


#endif
