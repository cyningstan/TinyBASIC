/*
 * Tiny BASIC
 * Token Handling Header
 *
 * Copyright (C) Damian Walker 2019
 * Created: 15-Aug-2019
 */


#ifndef __TOKEN_H__
#define __TOKEN_H__


/*
 * Type Declarations
 */


/* token classes */
typedef enum
  {
   TOKEN_NONE, /* no token has yet been identified */
   TOKEN_EOF, /* end of file */
   TOKEN_EOL, /* end of line */
   TOKEN_WORD, /* an identifier or keyword - token to be removed */
   TOKEN_NUMBER, /* a numeric constant */
   TOKEN_SYMBOL, /* a legal symbol - token to be removed */
   TOKEN_STRING, /* a string constant */
   TOKEN_LET, /* the LET keyword */
   TOKEN_IF, /* the IF keyword */
   TOKEN_THEN, /* the THEN keyword */
   TOKEN_GOTO, /* the GOTO keyword */
   TOKEN_GOSUB, /* the GOSUB keyword */
   TOKEN_RETURN, /* the RETURN keyword */
   TOKEN_END, /* the END keyword */
   TOKEN_PRINT, /* the PRINT keyword */
   TOKEN_INPUT, /* the INPUT keyword */
   TOKEN_REM, /* the REM keyword */
   TOKEN_VARIABLE, /* a single letter A..Z */
   TOKEN_PLUS, /* addition or unary positive */
   TOKEN_MINUS, /* subtraction or unary negative */
   TOKEN_MULTIPLY, /* multiplication */
   TOKEN_DIVIDE, /* division */
   TOKEN_LEFT_PARENTHESIS, /* open parenthesis */
   TOKEN_RIGHT_PARENTHESIS, /* close parenthesis */
   TOKEN_EQUAL, /* = */
   TOKEN_UNEQUAL, /* <> or >< */
   TOKEN_LESSTHAN, /* < */
   TOKEN_LESSOREQUAL, /* <= */
   TOKEN_GREATERTHAN, /* > */
   TOKEN_GREATEROREQUAL, /* >= */
   TOKEN_COMMA, /* comma separator */
   TOKEN_ILLEGAL /* unrecognised characters */
  } TokenClass;

/* token structure */
typedef struct token Token;
typedef struct token
{
  void *data; /* private data */
  void (*destroy) (Token *); /* destructor */
  void initialise (Token *, TokenClass, int, int, char *);
} Token;


/*
 * Function Declarations
 */


/*
 * Set the token's text content
 * params:
 *   Token*   token     the token to alter
 *   char*    content   the text content to set
 */
void token_set_content (Token *token, char *content);

/*
 * Set all of the values of an existing token in a single function call.
 * params:
 *   Token*       token     the token to update
 *   TokenClass   class     class of token to initialise
 *   int          line      line on which the token occurred
 *   int          pos       character position on which the token occurred
 *   char*        content   the textual content of the token
 */
void token_initialise (Token *token, TokenClass class, int line, int pos,
  char *content);

/*
 * Token constructor without values to initialise
 * returns:
 *   Token*   the created token
 */
Token *token_create (void);

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
Token *token_create_initialise (TokenClass class, int line, int pos, char *content);

/*
 * Token destructor
 * params:
 *   Token*   token   the doomed token
 */
void token_destroy (Token *token);

/*
 * Token debug output
 * params:
 *   Token*   token   the token to output
 */
void token_debug_output (Token *token);


#endif
