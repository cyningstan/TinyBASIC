/*
 * Tiny BASIC
 * Parser Header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 08-Aug-2019
 */


#ifndef __PARSER_H__
#define __PARSER_H__


/* pre-requisite headers */
#include "statement.h"


/*
 * Function Declarations
 */


/*
 * Parse the whole program
 * params:
 *   FILE*   input   the input file
 * returns:
 *   ProgramNode*    a pointer to the whole program
 */
ProgramNode *parse_program (FILE *input);


#endif
