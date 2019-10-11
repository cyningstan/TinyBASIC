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
 * Type Declarations
 */


/* The token stream */
typedef struct token_stream TokenStream; /* it contains self-references */
typedef struct token_stream {
  void *private_data; /* internal data for the token stream */
  Token *(*next) (TokenStream *);
  int (*get_line) (TokenStream *);
  void (*destroy) (TokenStream *);
} TokenStream;


/*
 * Function Declarations
 */


/*
 * Constructor for the token stream
 * params:
 *   FILE*   input   the input file stream
 * returns:
 *   TokenStream*    the token stream to read from
 */
TokenStream *new_TokenStream (FILE *input);


#endif
