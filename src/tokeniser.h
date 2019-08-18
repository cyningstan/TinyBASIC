/*
 * Tiny BASIC
 * Tokenisation Header
 *
 * Copyright (C) Damian Walker 2019
 * Created: 04-Aug-2019
 */


#ifndef __TOKENISER_H__
#define __TOKENISER_H__


/* pre-requisite headers */
#include "token.h"


/*
 * Function Declarations
 */


/*
 * Get the next token
 * globals:
 *   int     start_line   line on which the current token started
 *   int     start_pos    char pos on which the current token started
 * params:
 *   FILE*   input        handle for the input file
 * returns:
 *   Token*               the token built
 */
Token *tokeniser_next_token (FILE *input);

/*
 * Getter for the current line number
 * globals:
 *   int   line   the current line number
 * returns:
 *   int          the current line number returned
 */
int tokeniser_get_line (void);


#endif
