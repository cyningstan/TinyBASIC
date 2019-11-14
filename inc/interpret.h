/*
 * Tiny BASIC Interpreter and Compiler Project
 * Interpreter header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 23-Aug-2019
 */


#ifndef __INTERPRET_H__
#define __INTERPRET_H__


/* included headers */
#include "errors.h"
#include "options.h"


/*
 * Function Declarations
 */


/*
 * Interpret the program from the beginning
 * params:
 *   ProgramNode*       program           the program to interpret
 *   ErrorHandler*      runtime_errors    runtime error handler
 *   LanguageOptions*   runtime_options   language options
 */
void interpret_program (ProgramNode *program, ErrorHandler *runtime_errors,
  LanguageOptions *runtime_options);


#endif
