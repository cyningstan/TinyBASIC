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

/* language options */
typedef struct {
  LineNumberOption line_numbers; /* mandatory, implied, optional */
  int line_limit; /* highest line number allowed */
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


#endif