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
 * Parse the whole program
 * params:
 *   FILE*   input   the input file
 * returns:
 *   ProgramNode*    a pointer to the whole program
 */
ProgramNode *parse_program (FILE *input);

/*
 * Return the current source line we're parsing
 * globals:
 *   int   current_line   the line stored when the last token was read
 * returns:
 *   int                  the line returned
 */
int parser_line (void);

/*
 * Return the label of the source line we're parsing
 * globals:
 *   int   last_label   the label stored when validated
 * returns:
 *   int                the label returned
 */
int parser_label (void);


#endif
