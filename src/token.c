/*
 * Tiny BASIC
 * Token handling module
 *
 * Copyright (C) Damian Gareth Walker 2019
 * Created: 15-Aug-2019
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"


/*
 * Public methods
 */


/*
 * Set the token's text content
 * params:
 *   Token*   token     the token to alter
 *   char*    content   the text content to set
 */
static void set_content (Token *token, char *content) {
  if (token->content)
    free (token->content);
  token->content = malloc (strlen (content) + 1);
  strcpy (token->content, content);
}

/*
 * Set all of the values of an existing token in a single function call.
 * params:
 *   Token*   token   the token to update
 *   TokenClass   class     class of token to initialise
 *   int          line      line on which the token occurred
 *   int          pos       character position on which the token occurred
 *   char*        content   the textual content of the token
 */
static void initialise (Token *token, TokenClass class, int line, int pos,
  char *content) {

  /* initialise the easy members */
  token->class = class ? class : TOKEN_NONE;
  token->line = line ? line : 0;
  token->pos = pos ? pos : 0;

  /* initialise the content */
  if (content)
    token->set_content (token, content);
}

/*
 * Token destructor
 * params:
 *   Token*   token   the doomed token
 */
static void destroy (Token *token) {
  if (token->content)
    free (token->content);
  free (token);
}


/*
 * Constructors
 */


/*
 * Token constructor without values to initialise
 * returns:
 *   Token*   the created token
 */
Token *new_Token (void) {

  /* local variables */
  Token *token; /* the token to create */

  /* create the structure and initialise its members */
  token = malloc (sizeof (Token));
  token->class = TOKEN_NONE;
  token->line = token->pos = 0;
  token->content = NULL;
  token->set_content = set_content;
  token->initialise = initialise;
  token->destroy = destroy;

  /* return the created structure */
  return token;
}

/*
 * Token constructor with values to initialise
 * params:
 *   TokenClass   class     class of token to initialise
 *   int          line      line on which the token occurred
 *   int          pos       character position on which the token occurred
 *   char*        content   the textual content of the token
 * returns:
 *   Token*                 the created token
 */
Token *new_Token_initialise (TokenClass class, int line, int pos,
  char *content) {

  /* local variables */
  Token *token; /* token to create */

  /* create a blank token */
  token = new_Token ();
  token->content = NULL;
  token->initialise (token, class, line, pos, content);

  /* return the new token */
  return token;
}
