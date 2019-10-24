/*
 * Tiny BASIC
 * Token handling functions
 *
 * Copyright (C) Damian Walker 2019
 * Created: 15-Aug-2019
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"


/*
 * Internal data structures
 */


/* Private data */
typedef struct {
  TokenClass class; /* class of token */
  int line; /* line on which token was found */
  int pos; /* position within the line on which token was found */
  char *content; /* representation of token */
} Private;


/*
 * Internal Data
 */


/* Convenience variables */
Token *this;
Private *data;


/*
 * Level 2 Functions
 */


/*
 * Set the token's text content
 * params:
 *   Token*   token     the token to alter
 *   char*    content   the text content to set
 */
void token_set_content (Token *token, char *content) {
  if (token->content)
    free (token->content);
  token->content = malloc (strlen (content) + 1);
  strcpy (token->content, content);
}


/*
 * Level 1 Functions
 */


/*
 * Public methods
 */


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
    token_set_content (token, content);
}

/*
 * Set the token class
 * params:
 *   Token*       token   the token to set
 *   TokenClass   class   the class
 */
static void set_class (Token *token, TokenClass *class) {
}

/*
 * Token destructor
 * params:
 *   Token*   token   the doomed token
 */
static void destroy (Token *token) {
  if ((this = token)) {
    data = this->data;
    if (data->content)
      free (data->content);
    free (data);
    free (this);
  }
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

  /* create the structure */
  this = malloc (sizeof (Token));
  this->data = data = malloc (sizeof (Private));

  /* initialise properties */
  data->class = TOKEN_NONE;
  data->line = data->pos = 0;
  data->content = NULL;

  /* set up methods */
  this->initialise = initialise;
  this->destroy = destroy;

  /* return the created structure */
  return this;
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
Token *new_Token_initialise
  (TokenClass class, int line, int pos, char *content) {

  /* create a blank token */
  this = token_create ();
  this->initialise (this, class, line, pos, content);

  /* return the new token */
  return this;
}
