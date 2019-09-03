/*
 * Tiny BASIC
 * Tokenisation module
 *
 * Copyright (C) Damian Gareth Walker 2019
 * Created: 04-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "tokeniser.h"


/*
 * Data definitions
 */


/* modes of reading */
typedef enum {
  DEFAULT_MODE, /* we have no idea what's coming */
  COMMENT_MODE, /* reading a comment */
  WORD_MODE, /* reading an identifier or keyword */
  NUMBER_MODE, /* reading a numeric constant */
  LESS_THAN_MODE, /* reading an operator staring with < */
  GREATER_THAN_MODE, /* reading an operator starting with > */
  STRING_LITERAL_MODE, /* reading a string literal */
  UNKNOWN_MODE /* we are lost */
} Mode;

/* current state information */
typedef struct {
  Token *token; /* token to return */
  FILE *input; /* the input file */
  Mode mode; /* current reading mode */
  int ch; /* last-read character */
  char *content; /* content of token under construction */
  int max; /* memory reserved for content */
} TokeniserState;

/* global variables */
static int line = 1; /* current line in the input file */
static int pos = 0; /* current position on the input line */
static int start_line = 1; /* line on which a token started */
static int start_pos = 0; /* position on which a token started */


/*
 * Level 2 Tokeniser Routines
 */


/*
 * Read a character and update the position counter
 * globals:
 *   int               line    current line after character read
 *   int               pos     current character position after character read
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 * returns:
 *   int              character just read
 */
int tokeniser_read_character (TokeniserState *state) {

  int ch; /* character read from stream */

  /* read the character */
  ch = fgetc (state->input);

  /* update the position and line counters */
  if (ch == '\n') {
    ++line;
    pos = 0;
  } else {
    ++pos;
  }

  /* return the character */
  return ch;
}

/* 
 * Push a character back into the input stream and update position markers
 * globals:
 *   int               line    line number rolled back
 *   int               pos     character position rolled back
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 */
void tokeniser_unread_character (TokeniserState *state) {
  ungetc (state->ch, state->input);
  if (state->ch == '\n')
    --line;
  else
    --pos;
}

/*
 * Append the last read character to the token content
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 */
void tokeniser_store_character (TokeniserState *state) {

  /* variable declarations */
  char *temp; /* temporary pointer to content */
  int length; /* current length of token */

  /* allocate more memory for the token content if necessary */
  if (strlen (state->content) == state->max - 1) {
    temp = state->content;
    state->max *= 2;
    state->content = malloc (state->max);
    strcpy (state->content, temp);
    free (temp);
  }

  /* now add the character to the token */
  length = strlen (state->content);
  state->content [length++] = state->ch;
  state->content [length] = '\0';

}


/*
 * Level 1 Tokeniser Routines
 */


/*
 * Default mode - deal with character when state is unknown
 * globals:
 *   int               line         current line in the source file
 *   int               pos          current character position in the source file
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_default_mode (TokeniserState *state) {

  /* deal with whitespace */
  if (state->ch == ' ' ||
      state->ch == '\t' ||
      state->ch == '\r' ||
      state->ch == '\n') {
    state->ch = tokeniser_read_character (state);
    start_line = line;
    start_pos = pos;
  }

  /* alphabetic characters start a word */
  else if ((state->ch >= 'A' && state->ch <= 'Z') ||
	   (state->ch >= 'a' && state->ch <= 'z')) {
    start_line = line;
    start_pos = pos;
    state->mode = WORD_MODE;
  }

  /* digits start a number */
  else if (state->ch >= '0' && state->ch <= '9')
    state->mode = NUMBER_MODE;

  /* check for tokens starting with less-than (<, <=, <>) */
  else if (state->ch == '<') {
    start_line = line;
    start_pos = pos;
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
    state->mode = LESS_THAN_MODE;
  }

  /* check for tokens starting with greater-than (>, >=) */
  else if (state->ch == '>') {
    start_line = line;
    start_pos = pos;
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
    state->mode = GREATER_THAN_MODE;
  }

  /* deal with other symbol operators */
  else if (strchr ("+-*/=(),", state->ch) != NULL) {
    start_line = line;
    start_pos = pos;
    tokeniser_store_character (state);
    state->token = token_create_initialise (TOKEN_SYMBOL, start_line, start_pos,
      state->content);
  }

  /* double quotes start a string literal */
  else if (state->ch == '"') {
    start_line = line;
    start_pos = pos;
    state->ch = tokeniser_read_character (state);
    state->mode = STRING_LITERAL_MODE;
  }

  /* detect end of file */
  else if (state->ch == EOF) {
    start_line = line;
    start_pos = pos;
    state->token = token_create_initialise (TOKEN_EOF, start_line, start_pos,
      state->content);
  }

  /* other characters are illegal */
  else {
    start_line = line;
    start_pos = pos;
    tokeniser_store_character (state);
    state->token = token_create_initialise (TOKEN_ILLEGAL, start_line, start_pos,
      state->content);
  }

}

/*
 * Word mode - deal with character when building a word token
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_word_mode (TokeniserState *state) {
    
  /* add letters and digits to the token */
  if ((state->ch >= 'A' && state->ch <= 'Z') ||
      (state->ch >= 'a' && state->ch <= 'z')) {
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
  }
    
  /* other characters are pushed back for the next token */
  else {
    if (state->ch != EOF)
      tokeniser_unread_character (state);
    if (strcmp (state->content, "REM"))
      state->token = token_create_initialise
        (TOKEN_WORD, start_line, start_pos, state->content);
    else {
      *state->content = '\0';
      state->mode = COMMENT_MODE;
    }
  }
    
}

/*
 * Comment mode - skip till end of line after a REM
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_comment_mode (TokeniserState *state) {
  if (state->ch == '\n')
    state->mode = DEFAULT_MODE;
  else
    state->ch = tokeniser_read_character (state);
}

/*
 * Number mode - building a number token (integer only)
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_number_mode (TokeniserState *state) {

  /* add digits to the token */
  if (state->ch >= '0' && state->ch <= '9') {
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
  }
    
  /* other characters are pushed back for the next token */
  else {
    if (state->ch != EOF)
      tokeniser_unread_character (state);
    state->token = token_create_initialise
      (TOKEN_NUMBER, start_line, start_pos, state->content);
  }

}

/*
 * Less than mode - checking for <> and <= operators
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_less_than_mode (TokeniserState *state) {
  if (state->ch == '=' || state->ch == '>')
    tokeniser_store_character (state);
  else
    tokeniser_unread_character (state);
  state->token = token_create_initialise
    (TOKEN_SYMBOL, start_line, start_pos, state->content);
}

/*
 * Greater than mode - checking for >= and >< operators
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_greater_than_mode (TokeniserState *state) {
  if (state->ch == '=' || state->ch == '<')
    tokeniser_store_character (state);
  else
    ungetc (state->ch, state->input);
  state->token = token_create_initialise
    (TOKEN_SYMBOL, start_line, start_pos, state->content);
}

/*
 * String literal mode - reading a string
 * globals:
 *   int               start_line   line on which the current token started
 *   int               start_pos    char pos on which the current token started
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
void tokeniser_string_literal_mode (TokeniserState *state) {

  /* a quote terminates the string */
  if (state->ch == '"')
    state->token = token_create_initialise
      (TOKEN_STRING, start_line, start_pos, state->content);

  /* a backslash escapes the next character */
  else if (state->ch == '\\') {
    state->ch = tokeniser_read_character (state);
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
  }

  /* EOF generates an error */
  else if (state->ch == EOF)
    state->token = token_create_initialise
      (TOKEN_ILLEGAL, start_line, start_pos, state->content);

  /* all other characters are part of the string */
  else {
    tokeniser_store_character (state);
    state->ch = tokeniser_read_character (state);
  }
}


/*
 * Top Level Tokeniser Routines
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
Token *tokeniser_next_token (FILE *input) {

  /* local variables */
  TokeniserState state; /* current state of reading */
  Token *return_token; /* token to return */

  /* initialise the state */
  state.token = NULL;
  state.input = input;
  state.mode = DEFAULT_MODE;
  state.max = 1024;
  state.content = malloc (state.max);
  *(state.content) = '\0';
  state.ch = tokeniser_read_character (&state);

  /* main loop */
  while (state.token == NULL) {
    switch (state.mode) {
    case DEFAULT_MODE:
      tokeniser_default_mode (&state);
      break;
    case COMMENT_MODE:
      tokeniser_comment_mode (&state);
      break;
    case WORD_MODE:
      tokeniser_word_mode (&state);
      break;
    case NUMBER_MODE:
      tokeniser_number_mode (&state);
      break;
    case LESS_THAN_MODE:
      tokeniser_less_than_mode (&state);
      break;
    case GREATER_THAN_MODE:
      tokeniser_greater_than_mode (&state);
      break;
    case STRING_LITERAL_MODE:
      tokeniser_string_literal_mode (&state);
      break;
    default:
      token_initialise (state.token, TOKEN_EOF, start_line, start_pos,
        state.content);
      state.ch = EOF; /* temporary hack */
    }
  }

  /* store token and release state memory */
  return_token = state.token;
  free (state.content);

  /* return result */
  return return_token;

}

/*
 * Getter for the current line number
 * globals:
 *   int   line   the current line number
 * returns:
 *   int          the current line number returned
 */
int tokeniser_get_line (void) {
  return line;
}
