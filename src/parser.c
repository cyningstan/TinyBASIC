/*
 * Tiny BASIC Interpreter and Compiler Project
 * Parser module
 *
 * Copyright (C) Damian Gareth Walker 2019
 * Created: 12-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "options.h"
#include "token.h"
#include "tokeniser.h"
#include "parser.h"
#include "expression.h"


/*
 * Internal Function Declarations
 */


/* parse_expression() has a forward reference from parse_factor() */
ExpressionNode *parse_expression (void);


/*
 * Data Definitions
 */


/* global variables */
static int last_label = 0; /* last line label encountered */
static FILE *input; /* the input file */
static Token *stored_token = NULL; /* token read ahead */


/*
 * Level 5 Parser Routines
 */


/*
 * Get next token to parse, from read-ahead buffer or tokeniser.
 * globals:
 *   Token   *stored_token   a token already read in.
 */
Token *get_token_to_parse () {

  /* local variables */
  Token *token; /* token to return */

  /* get the token one way or another */
  if (stored_token) {
    token = stored_token;
    stored_token = NULL;
  } else
    token = tokeniser_next_token (input);

  /* return the token */
  return token;
}


/*
 * Level 4 Parser Routines
 */


/*
 * Parse a factor
 * returns:
 *   FactorNode*   a new factor node holding the parsed data
 */
FactorNode *parse_factor (void) {

  /* local variables */
  Token *token; /* token to read */
  FactorNode *factor = NULL; /* the factor we're building */
  ExpressionNode *expression = NULL; /* any parenthesised expression */

  /* initialise the factor and grab the next token */
  factor = factor_create ();
  token = get_token_to_parse ();

  /* interpret a sign */
  if (token->class == TOKEN_SYMBOL
    && (! strcmp (token->content, "+")
    || ! strcmp (token->content, "-"))) {
    factor->sign = (*token->content == '+')
      ? SIGN_POSITIVE
      : SIGN_NEGATIVE;
    token = get_token_to_parse ();
  }

  /* interpret a number */
  if (token->class == TOKEN_NUMBER) {
    factor->class = FACTOR_VALUE;
    sscanf (token->content, "%d", &factor->data.value);
  }

  /* interpret a variable */
  else if (token->class == TOKEN_WORD
    && strlen (token->content) == 1
    && toupper (*token->content) >= 'A'
    && toupper (*token->content) <= 'Z') {
    factor->class = FACTOR_VARIABLE;
    factor->data.variable = (int) *token->content & 0x1F;
  }

  /* interpret an parenthesised expression */
  else if (token->class == TOKEN_SYMBOL
    && ! strcmp (token->content, "(")) {

    /* if expression is valid, check for ")" and complete the factor */
    expression = parse_expression ();
    if (expression) {
      token = get_token_to_parse ();
      if (token->class == TOKEN_SYMBOL
        && ! strcmp (token->content, ")")) {
        factor->class = FACTOR_EXPRESSION;
        factor->data.expression = expression;
      } else {
        errors_set_code (E_MISSING_RIGHT_PARENTHESIS);
        factor_destroy (factor);
        factor = NULL;
        expression_destroy (expression);
      }
    }

    /* clean up after invalid parenthesised expression */
    else {
      errors_set_code (E_INVALID_EXPRESSION);
      factor_destroy (factor);
      factor = NULL;
    }
  }

  /* deal with other errors */
  else {
    errors_set_code (E_INVALID_EXPRESSION);
    factor_destroy (factor);
    factor = NULL;
  }

  /* return the factor */
  return factor;
}


/*
 * Level 3 Parser Routines
 */


/*
 * Parse a term
 * returns:
 *   TermNode*   a new term node holding the parsed term
 */
TermNode *parse_term (void) {

  /* local variables */
  TermNode *term = NULL; /* the term we're building */
  FactorNode *factor = NULL; /* factor detected */

  /* temporary code to accept only a single factor */
  if ((factor = parse_factor ())) {
    term = term_create ();
    term->factor = factor;
    term->next = NULL;
  }

  /* return the evaluated term, if any */
  return term;
}


/*
 * Level 2 Parser Routines
 */


/*
 * Parse an expression
 * returns:
 *   ExpressionNode*   a new expression node holding the parsed expression
 */
ExpressionNode *parse_expression (void) {

  /* local variables */
  ExpressionNode *expression = NULL; /* the expression we're building */
  TermNode *term; /* term detected */

  /* temporary code: accept only a single term */
  if ((term = parse_term ())) {
    expression = expression_create ();
    expression->term = term;
    expression->next = NULL;
  }

  /* return the evaluated expression, if any */
  return expression;
}


/*
 * Level 1 Parser Routines
 */


/*
 * Calculate numeric line label according to language options.
 * This will be used if the line has no label specified.
 * returns:
 *   int                         numeric line label
 */
int generate_default_label (void) {
  if (options_get ().line_numbers == LINE_NUMBERS_IMPLIED)
    return last_label + 1;
  else
    return 0;
}


/*
 * Validate a line label according to the language options
 * params:
 *   int               label     the numeric label to verify.
 * returns:
 *   int                         !0 if the number is valid, 0 if invalid
 */
int validate_line_label (int label) {

  /* line labels should be non-negative and within the set limit */
  if (label < 0 || label > options_get ().line_limit)
    return 0;

  /* line labels should be non-zero unless they're completely optional */
  if (label == 0 && options_get ().line_numbers != LINE_NUMBERS_OPTIONAL)
    return 0;

  /* line labels should be ascending unless they're completely optional */
  if (label <= last_label
    && options_get ().line_numbers != LINE_NUMBERS_OPTIONAL)
    return 0;

  /* if all the above tests passed, the line label is valid */
  return 1;
}


/*
 * Identify a statement from its initial keyword
 * params:
 *   Token            *token   the first token in a statement
 * return:
 *   StatementClass            the class of statement identified
 */
StatementClass get_statement_class (Token *token) {

  /* local variables */
  char *uccontent, /* contents of the token in upper case */
    *ucptr; /* pointer for upper case conversion */
  StatementClass class; /* class identified */

  /* check that this is a word */
  if (token->class != TOKEN_WORD)
    return STATEMENT_NONE;

  /* create an upper case copy */
  uccontent = malloc (strlen (token->content) + 1);
  strcpy (uccontent, token->content);
  for (ucptr = uccontent; *ucptr; ++ucptr)
    *ucptr = toupper (*ucptr);

  /* identify the command */
  if (! strcmp (uccontent, "LET"))
    class = STATEMENT_LET;
  else if (! strcmp (uccontent, "IF"))
    class = STATEMENT_IF;
  else if (! strcmp (uccontent, "GOTO"))
    class = STATEMENT_GOTO;
  else if (! strcmp (uccontent, "GOSUB"))
    class = STATEMENT_GOSUB;
  else if (! strcmp (uccontent, "RETURN"))
    class = STATEMENT_RETURN;
  else if (! strcmp (uccontent, "END"))
    class = STATEMENT_END;
  else if (! strcmp (uccontent, "PRINT"))
    class = STATEMENT_PRINT;
  else if (! strcmp (uccontent, "INPUT"))
    class = STATEMENT_INPUT;
  else
    class = STATEMENT_NONE;

  /* return the identified class */
  free (uccontent);
  return class;
}


/*
 * Skip the remainder of a line.
 * This is required during development only.
 * The final release will process all the lines fully.
 * globals:
 *   Token   *stored_token   Stores first token on next line.
 */
void _skip_line (void) {

  /* variable declarations */
  int line;
  Token *token = NULL;
  TokenClass class;

  /* retrieve last line */
  line = tokeniser_get_line ();

  /* read tokens till we hit the next line, or EOF */
  do {
    if (token) token_destroy (token);
    token = get_token_to_parse (input);
    class = token->class;
  } while (tokeniser_get_line () == line && class != TOKEN_EOF);

  /* store the first token from the next line */
  if (class == TOKEN_EOF)
    token_destroy (token);
  else
    stored_token = token;
}


/*
 * Parse a LET statement
 * params:
 *   StatementNode   *statement   The statement to assemble.
 */
void parse_let_statement (StatementNode *statement) {

  /* local variables */
  Token *token; /* tokens read as part of LET statement */
  int line; /* line containing the LET token */

  /* initialise the statement */
  statement->class = STATEMENT_LET;
  statement->statement.letn = malloc (sizeof (LetStatementNode));
  statement->statement.letn->variable = 0;
  statement->statement.letn->expression = NULL;
  line = tokeniser_get_line ();

  /* see what variable we're assigning */
  token = get_token_to_parse ();
  if (token->class != TOKEN_WORD) {
    errors_set_code (E_INVALID_VARIABLE);
    return;
  } else if (strlen (token->content) != 1) {
    errors_set_code (E_INVALID_VARIABLE);
    return;
  } else if (toupper (*token->content) < 'A'
    || toupper (*token->content) > 'Z') {
    errors_set_code (E_INVALID_VARIABLE);
    return;
  }
  statement->statement.letn->variable = toupper(*token->content) - 'A' + 1;

  /* get the "=" */
  token_destroy (token);
  token = get_token_to_parse ();
  if (token->class != TOKEN_SYMBOL) {
    errors_set_code (E_INVALID_ASSIGNMENT);
    return;
  } else if (strcmp (token->content, "=")) {
    errors_set_code (E_INVALID_ASSIGNMENT);
    return;
  }

  /* get the expression */
  statement->statement.letn->expression = parse_expression ();
  if (! statement->statement.letn->expression)
    errors_set_code (E_INVALID_EXPRESSION);

  /* check for a line end */
  line = token->line;
  stored_token = token = get_token_to_parse ();
  if (token->class != TOKEN_EOF && token->line == line)
    errors_set_code (E_INVALID_EXPRESSION);
}


/*
 * Destructor for a LET statement
 * params:
 *   LetStatementNode   *letn   the doomed LET statement.
 */
void destroy_let_statement (LetStatementNode *letn) {
  if (letn->expression)
    expression_destroy (letn->expression);
  free (letn);
}


/*
 * Top Level Parser Routines
 */


/*
 * Parse a single statement from the source file.
 * globals:
 *   Token             *stored_token   a token already pre-read.
 *   int               last_label      last line label used.
 * params:
 *   FILE              *fh             file handle for the source file.
 * returns:
 *   StatementNode                     a fully-assembled statement, hopefully.
 */
StatementNode *get_next_statement (FILE *fh) {

  /* local variables */
  Token *token; /* token read */
  StatementNode *statement; /* statement read */
  StatementClass class; /* class of statement identified but not validated */

  /* initialise the statement */
  input = fh;
  statement = malloc (sizeof (StatementNode));
  statement->label = generate_default_label ();
  statement->class = STATEMENT_NONE;
  statement->next = NULL;

  /* get the first token */
  token = get_token_to_parse ();

  /* deal with end of file */
  if (token->class == TOKEN_EOF) {
    token_destroy (token);
    statement_destroy (statement);
    return NULL;
  }

  /* deal with line label, if supplied */
  if (token->class == TOKEN_NUMBER) {
    sscanf(token->content, "%d", &statement->label);
    token_destroy (token);
    token = get_token_to_parse (input);
  }

  /* validate the supplied or implied line label */
  if (validate_line_label (statement->label))
    last_label = statement->label;
  else {
    errors_set_code (E_INVALID_LINE_NUMBER);
    token_destroy (token);
    statement_destroy (statement);
    return NULL;
  }

  /* check for command */
  switch ((class = get_statement_class (token))) {
  case STATEMENT_LET:
    parse_let_statement (statement);
    break;
  case STATEMENT_IF:
  case STATEMENT_GOTO:
  case STATEMENT_GOSUB:
  case STATEMENT_RETURN:
  case STATEMENT_END:
  case STATEMENT_PRINT:
  case STATEMENT_INPUT:
    statement->class = class;
    _skip_line ();
    break;
  default:
    errors_set_code (E_UNRECOGNISED_COMMAND);
    token_destroy (token);
    statement_destroy (statement);
    return NULL;
  }

  /* destroy used-up token and return the statement */
  if (! stored_token)
    token_destroy (token);
  return statement;
}