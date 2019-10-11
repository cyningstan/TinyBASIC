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
#include "common.h"


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

/* private data for token stream */
typedef struct {
  FILE *input; /* the input file handle */
  int line; /* current line in the input file */
  int pos; /* current position on the input line */
  int start_line; /* line on which a token started */
  int start_pos; /* position on which a token started */
} Private;

/* file scope variables */
static TokenStream *this; /* convenience variable for current object */
static Private *data; /* convenience variable for private data */


/*
 * Private Methods
 */


/*
 * Read a character and update the position counter
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 * returns:
 *   int              character just read
 */
static int read_character (TokeniserState *state) {

  /* local variables */
  int ch; /* character read from stream */

  /* read the character */
  ch = fgetc (data->input);

  /* update the position and line counters */
  if (ch == '\n') {
    ++data->line;
    data->pos = 0;
  } else {
    ++data->pos;
  }

  /* return the character */
  return ch;
}

/* 
 * Push a character back into the input stream and update position markers
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 */
static void unread_character (TokeniserState *state) {
  ungetc (state->ch, data->input);
  if (state->ch == '\n')
    --data->line;
  else
    --data->pos;
}

/*
 * Append the last read character to the token content
 * params:
 *   TokeniserState*   state   current state of the tokeniser
 */
static void store_character (TokeniserState *state) {

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
 * Identify the various recognised symbols
 * params:
 *   int   ch     the character to identify
 * returns:
 *   TokenClass   the token class recognised by the parser
 */
static TokenClass identify_symbol (int ch) {
  switch (ch) {
  case '+':
    return TOKEN_PLUS;
    break;
  case '-':
    return TOKEN_MINUS;
    break;
  case '*':
    return TOKEN_MULTIPLY;
    break;
  case '/':
    return TOKEN_DIVIDE;
    break;
  case '=':
    return TOKEN_EQUAL;
    break;
  case '(':
    return TOKEN_LEFT_PARENTHESIS;
    break;
  case ')':
    return TOKEN_RIGHT_PARENTHESIS;
    break;
  case ',':
    return TOKEN_COMMA;
    break;
  default:
    return TOKEN_SYMBOL;
  }
}

/*
 * Attempt to identify a keyword
 * params:
 *   char*   word   the string to identify
 * returns:
 *   TokenClass     the token type of the keyword, or TOKEN_WORD
 */
static TokenClass identify_word (char *word) {
  if (strlen (word) == 1)
    return TOKEN_VARIABLE;
  else if (! tinybasic_strcmp (word, "LET"))
    return TOKEN_LET;
  else if (! tinybasic_strcmp (word, "IF"))
    return TOKEN_IF;
  else if (! tinybasic_strcmp (word, "THEN"))
    return TOKEN_THEN;
  else if (! tinybasic_strcmp (word, "GOTO"))
    return TOKEN_GOTO;
  else if (! tinybasic_strcmp (word, "GOSUB"))
    return TOKEN_GOSUB;
  else if (! tinybasic_strcmp (word, "RETURN"))
    return TOKEN_RETURN;
  else if (! tinybasic_strcmp (word, "END"))
    return TOKEN_END;
  else if (! tinybasic_strcmp (word, "PRINT"))
    return TOKEN_PRINT;
  else if (! tinybasic_strcmp (word, "INPUT"))
    return TOKEN_INPUT;
  else if (! tinybasic_strcmp (word, "REM"))
    return TOKEN_REM;
  else
    return TOKEN_WORD;
}

/*
 * Identify compound (multi-character) symbols.
 * Also identifies some single-character symbols that can form
 * the start of multi-character symbols.
 * params:
 *   char*   symbol   the symbol to identify
 * returns:
 *   TokenClass       the identification
 */
static TokenClass identify_compound_symbol (char *symbol) {
  if (! strcmp (symbol, "<>")
      || ! strcmp (symbol, "><"))
    return TOKEN_UNEQUAL;
  else if (! strcmp (symbol, "<"))
    return TOKEN_LESSTHAN;
  else if (! strcmp (symbol, "<="))
    return TOKEN_LESSOREQUAL;
  else if (! strcmp (symbol, ">"))
    return TOKEN_GREATERTHAN;
  else if (! strcmp (symbol, ">="))
    return TOKEN_GREATEROREQUAL;
  else
    return TOKEN_SYMBOL;
}

/*
 * Default mode - deal with character when state is unknown
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void default_mode (TokeniserState *state) {

  /* deal with non-EOL whitespace */
  if (state->ch == ' ' ||
      state->ch == '\t') {
    state->ch = read_character (state);
    data->start_line = data->line;
    data->start_pos = data->pos;
  }

  /* deal with EOL whitespace */
  else if (state->ch == '\n') {
    data->start_line = data->line;
    data->start_pos = data->pos;
    state->token = new_Token_initialise
      (TOKEN_EOL, data->start_line, data->start_pos, state->content);
  }

  /* alphabetic characters start a word */
  else if ((state->ch >= 'A' && state->ch <= 'Z') ||
	   (state->ch >= 'a' && state->ch <= 'z')) {
    data->start_line = data->line;
    data->start_pos = data->pos;
    state->mode = WORD_MODE;
  }

  /* digits start a number */
  else if (state->ch >= '0' && state->ch <= '9')
    state->mode = NUMBER_MODE;

  /* check for tokens starting with less-than (<, <=, <>) */
  else if (state->ch == '<') {
    data->start_line = data->line;
    data->start_pos = data->pos;
    store_character (state);
    state->ch = read_character (state);
    state->mode = LESS_THAN_MODE;
  }

  /* check for tokens starting with greater-than (>, >=) */
  else if (state->ch == '>') {
    data->start_line = data->line;
    data->start_pos = data->pos;
    store_character (state);
    state->ch = read_character (state);
    state->mode = GREATER_THAN_MODE;
  }

  /* deal with other symbol operators */
  else if (strchr ("+-*/=(),", state->ch) != NULL) {
    data->start_line = data->line;
    data->start_pos = data->pos;
    store_character (state);
    state->token = new_Token_initialise (identify_symbol (state->ch),
      data->start_line, data->start_pos, state->content);
  }

  /* double quotes start a string literal */
  else if (state->ch == '"') {
    data->start_line = data->line;
    data->start_pos = data->pos;
    state->ch = read_character (state);
    state->mode = STRING_LITERAL_MODE;
  }

  /* detect end of file */
  else if (state->ch == EOF) {
    data->start_line = data->line;
    data->start_pos = data->pos;
    state->token = new_Token_initialise (TOKEN_EOF, data->start_line,
      data->start_pos, state->content);
  }

  /* other characters are illegal */
  else {
    data->start_line = data->line;
    data->start_pos = data->pos;
    store_character (state);
    state->token = new_Token_initialise
      (TOKEN_ILLEGAL, data->start_line, data->start_pos, state->content);
  }
}

/*
 * Word mode - deal with character when building a word token
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void word_mode (TokeniserState *state) {

  /* local variables */
  TokenClass class; /* recognised class of keyword */
    
  /* add letters and digits to the token */
  if ((state->ch >= 'A' && state->ch <= 'Z') ||
      (state->ch >= 'a' && state->ch <= 'z')) {
    store_character (state);
    state->ch = read_character (state);
  }
    
  /* other characters are pushed back for the next token */
  else {
    if (state->ch != EOF)
      unread_character (state);
    class = identify_word (state->content);
    if (class == TOKEN_REM) {
      *state->content = '\0';
      state->mode = COMMENT_MODE;
    }
    else
      state->token = new_Token_initialise
        (class, data->start_line, data->start_pos, state->content);
  }

}

/*
 * Comment mode - skip till end of line after a REM
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void comment_mode (TokeniserState *state) {
  if (state->ch == '\n')
    state->mode = DEFAULT_MODE;
  else
    state->ch = read_character (state);
}

/*
 * Number mode - building a number token (integer only)
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void number_mode (TokeniserState *state) {

  /* add digits to the token */
  if (state->ch >= '0' && state->ch <= '9') {
    store_character (state);
    state->ch = read_character (state);
  }
    
  /* other characters are pushed back for the next token */
  else {
    if (state->ch != EOF)
      unread_character (state);
    state->token = new_Token_initialise
      (TOKEN_NUMBER, data->start_line, data->start_pos, state->content);
  }

}

/*
 * Less than mode - checking for <> and <= operators
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void less_than_mode (TokeniserState *state) {
  if (state->ch == '=' || state->ch == '>')
    store_character (state);
  else
    unread_character (state);
  state->token = new_Token_initialise
    (identify_compound_symbol (state->content), data->start_line,
     data->start_pos, state->content);
}

/*
 * Greater than mode - checking for >= and >< operators
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void greater_than_mode (TokeniserState *state) {
  if (state->ch == '=' || state->ch == '<')
    store_character (state);
  else
    ungetc (state->ch, state->input);
  state->token = new_Token_initialise
    (identify_compound_symbol (state->content), data->start_line,
     data->start_pos, state->content);
}

/*
 * String literal mode - reading a string
 * params:
 *   TokeniserState*   state        current state of the tokeniser
 */
static void string_literal_mode (TokeniserState *state) {

  /* a quote terminates the string */
  if (state->ch == '"')
    state->token = new_Token_initialise
      (TOKEN_STRING, data->start_line, data->start_pos, state->content);

  /* a backslash escapes the next character */
  else if (state->ch == '\\') {
    state->ch = read_character (state);
    store_character (state);
    state->ch = read_character (state);
  }

  /* EOF generates an error */
  else if (state->ch == EOF)
    state->token = new_Token_initialise
      (TOKEN_ILLEGAL, data->start_line, data->start_pos, state->content);

  /* all other characters are part of the string */
  else {
    store_character (state);
    state->ch = read_character (state);
  }
}


/*
 * Public Methods
 */


/*
 * Get the next token
 * returns:
 *   Token*               the token built
 */
static Token *next (TokenStream *stream) {

  /* local variables */
  TokeniserState state; /* current state of reading */
  Token *return_token; /* token to return */

  /* initialise the convenience variables */
  this = stream;
  data = (Private *) stream->private_data;

  /* initialise the state */
  state.token = NULL;
  state.mode = DEFAULT_MODE;
  state.max = 1024;
  state.content = malloc (state.max);
  *(state.content) = '\0';
  state.ch = read_character (&state);

  /* main loop */
  while (state.token == NULL) {
    switch (state.mode) {
    case DEFAULT_MODE:
      default_mode (&state);
      break;
    case COMMENT_MODE:
      comment_mode (&state);
      break;
    case WORD_MODE:
      word_mode (&state);
      break;
    case NUMBER_MODE:
      number_mode (&state);
      break;
    case LESS_THAN_MODE:
      less_than_mode (&state);
      break;
    case GREATER_THAN_MODE:
      greater_than_mode (&state);
      break;
    case STRING_LITERAL_MODE:
      string_literal_mode (&state);
      break;
    default:
      state.token = new_Token_initialise (TOKEN_EOF, data->start_line,
	data->start_pos, state.content);
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
 * params:
 *   TokenStream*   stream   The token stream
 * returns:
 *   int                     the current line number returned
 */
static int get_line (TokenStream *stream) {
  data = (Private *) stream->private_data;
  return data->line;
}

/*
 * Destructor
 * params:
 *   TokenStream*   stream   The token stream
 */
static void destroy (TokenStream *stream) {
  if (stream) {
    if (stream->private_data)
      free (stream->private_data);
    free (stream);
  }
}


/*
 * Constructor
 */


/*
 * Constructor for the token stream
 * params:
 *   FILE*   input   the input file stream
 * returns:
 *   TokenStream*    the token stream to read from
 */
TokenStream *new_TokenStream (FILE *input) {

  /* initialise token stream */
  if ((this = malloc (sizeof (TokenStream)))) {
    this->private_data = data = malloc (sizeof (Private));
    data->input = input;
    data->line = 1;
    data->pos = 0;
    data->start_line = 1;
    data->start_pos = 0;
    this->next = next;
    this->get_line = get_line;
    this->destroy = destroy;
  }

  /* return the new token stream or NULL on failure */
  return this;
}
