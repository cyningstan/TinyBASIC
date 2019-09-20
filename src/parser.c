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
#include "common.h"
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

/* parse_statement() has a forward reference from parse_if_statement() */
StatementNode *parse_statement (void);


/*
 * Data Definitions
 */


/* global variables */
static int last_label = 0; /* last line label encountered */
static int current_line = 0; /* the last source line parsed */
static int end_of_file = 0; /* end of file signal */
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

  /* store the line, check EOF and return the token */
  current_line = token->line;
  if (token->class == TOKEN_EOF)
    end_of_file = !0;
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
  int start_line; /* the line on which this factor occurs */

  /* initialise the factor and grab the next token */
  factor = factor_create ();
  token = get_token_to_parse ();
  start_line = token->line;

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
    if (factor->data.value < -32768 || factor->data.value > 32767)
      errors_set_code (E_OVERFLOW, start_line, last_label);
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
        errors_set_code (E_MISSING_RIGHT_PARENTHESIS, start_line, 
          last_label);
        factor_destroy (factor);
        factor = NULL;
        expression_destroy (expression);
      }
    }

    /* clean up after invalid parenthesised expression */
    else {
      errors_set_code (E_INVALID_EXPRESSION, token->line, last_label);
      factor_destroy (factor);
      factor = NULL;
    }
  }

  /* deal with other errors */
  else {
    errors_set_code (E_INVALID_EXPRESSION, token->line, last_label);
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
 * globals:
 *   Token*      stored_token   the token read past the end of the term
 * returns:
 *   TermNode*   a new term node holding the parsed term
 */
TermNode *parse_term (void) {

  /* local variables */
  TermNode *term = NULL; /* the term we're building */
  FactorNode *factor = NULL; /* factor detected */
  RightHandFactor
    *rhptr = NULL, /* previous right-hand factor */
    *rhfactor = NULL; /* right-hand factor detected */
  Token *token = NULL; /* token read while looking for operator */

  /* scan the first factor */
  if ((factor = parse_factor ())) {
    term = term_create ();
    term->factor = factor;
    term->next = NULL;

    /* look for subsequent factors */
    while ((token = get_token_to_parse ())
      && ! errors_get_code ()
      && token->class == TOKEN_SYMBOL
      && strlen (token->content) == 1
      && (*token->content == '*'
      || *token->content == '/')) {

      /* parse the sign and the factor */
      rhfactor = rhfactor_create ();
      rhfactor->op = *token->content == '*'
          ? TERM_OPERATOR_MULTIPLY
          : TERM_OPERATOR_DIVIDE;
      if ((rhfactor->factor = parse_factor ())) {
        rhfactor->next = NULL;
        if (rhptr)
          rhptr->next = rhfactor;
        else
          term->next = rhfactor;
        rhptr = rhfactor;
      }

      /* set an error if we read an operator but not a factor */
      else {
        rhfactor_destroy (rhfactor);
        if (! errors_get_code ()) /* belt & braces */
          errors_set_code (E_INVALID_EXPRESSION, token->line, 
            last_label);
      }

      /* clean up token */
      token_destroy (token);
    }

    /* we've read past the end of the term; put the token back */
    stored_token = token;
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
 *   ExpressionNode*    the parsed expression
 */
ExpressionNode *parse_expression (void) {

  /* local variables */
  ExpressionNode *expression = NULL; /* the expression we build */
  TermNode *term; /* term detected */
  RightHandTerm
    *rhterm = NULL, /* the right-hand term detected */
    *rhptr = NULL; /* pointer to the previous right-hand term */
  Token *token; /* token read when scanning for right-hand terms */

  /* scan the first term */
  if ((term = parse_term ())) {
    expression = expression_create ();
    expression->term = term;
    expression->next = NULL;

    /* look for subsequent terms */
    while ((token = get_token_to_parse ())
      && ! errors_get_code ()
      && token->class == TOKEN_SYMBOL
      && strlen (token->content) == 1
      && (*token->content == '+'
      || *token->content == '-')) {

      /* parse the sign and the factor */
      rhterm = rhterm_create ();
      rhterm->op = *token->content == '+'
          ? EXPRESSION_OPERATOR_PLUS
          : EXPRESSION_OPERATOR_MINUS;
      if ((rhterm->term = parse_term ())) {
        rhterm->next = NULL;
        if (rhptr)
          rhptr->next = rhterm;
        else
          expression->next = rhterm;
        rhptr = rhterm;
      }

      /* set an error condition if we read a sign but not a factor */
      else {
        rhterm_destroy (rhterm);
        if (! errors_get_code ()) /* belt & braces */
          errors_set_code (E_INVALID_EXPRESSION, token->line, 
            last_label);
      }

      /* clean up token */
      token_destroy (token);
    }

    /* we've read past the end of the term; put the token back */
    stored_token = token;
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

  /* line labels should be non-zero unless they're optional */
  if (label == 0
    && options_get ().line_numbers != LINE_NUMBERS_OPTIONAL)
    return 0;

  /* line labels should be ascending unless they're optional */
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
  StatementClass class = STATEMENT_NONE; /* class identified */

  /* identify the command */
  if (token->class == TOKEN_EOL)
    class = STATEMENT_NONE;
  else if (token->class != TOKEN_WORD)
    errors_set_code (E_UNRECOGNISED_COMMAND, current_line, last_label);
  else if (! tinybasic_strcmp (token->content, "LET"))
    class = STATEMENT_LET;
  else if (! tinybasic_strcmp (token->content, "IF"))
    class = STATEMENT_IF;
  else if (! tinybasic_strcmp (token->content, "GOTO"))
    class = STATEMENT_GOTO;
  else if (! tinybasic_strcmp (token->content, "GOSUB"))
    class = STATEMENT_GOSUB;
  else if (! tinybasic_strcmp (token->content, "RETURN"))
    class = STATEMENT_RETURN;
  else if (! tinybasic_strcmp (token->content, "END"))
    class = STATEMENT_END;
  else if (! tinybasic_strcmp (token->content, "PRINT"))
    class = STATEMENT_PRINT;
  else if (! tinybasic_strcmp (token->content, "INPUT"))
    class = STATEMENT_INPUT;
  else
    errors_set_code (E_UNRECOGNISED_COMMAND, current_line, last_label);

  /* return the identified class */
  return class;
}


/*
 * Parse a LET statement
 * globals:
 *   Token*   stored_token   The token after the whole statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_let_statement (void) {

  /* local variables */
  Token *token; /* tokens read as part of LET statement */
  int line; /* line containing the LET token */
  StatementNode *statement; /* the new statement */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_LET;
  statement->statement.letn = statement_create_let ();
  line = tokeniser_get_line ();

  /* see what variable we're assigning */
  token = get_token_to_parse ();
  if (token->class != TOKEN_WORD) {
    errors_set_code (E_INVALID_VARIABLE, line, last_label);
    statement_destroy (statement);
    return NULL;
  } else if (strlen (token->content) != 1) {
    errors_set_code (E_INVALID_VARIABLE, line, last_label);
    statement_destroy (statement);
    return NULL;
  } else if (toupper (*token->content) < 'A'
    || toupper (*token->content) > 'Z') {
    errors_set_code (E_INVALID_VARIABLE, line, last_label);
    statement_destroy (statement);
    return NULL;
  }
  statement->statement.letn->variable
    = toupper(*token->content) - 'A' + 1;

  /* get the "=" */
  token_destroy (token);
  token = get_token_to_parse ();
  if (token->class != TOKEN_SYMBOL) {
    errors_set_code (E_INVALID_ASSIGNMENT, line, last_label);
    statement_destroy (statement);
    return NULL;
  } else if (strcmp (token->content, "=")) {
    errors_set_code (E_INVALID_ASSIGNMENT, line, last_label);
    statement_destroy (statement);
    return NULL;
  }

  /* get the expression */
  statement->statement.letn->expression = parse_expression ();
  if (! statement->statement.letn->expression) {
    errors_set_code (E_INVALID_EXPRESSION, line, last_label);
    statement_destroy (statement);
    return NULL;
  }

  /* return the statement */
  return statement;
}


/*
 * Parse an IF statement
 * globals:
 *   Token*   stored_token   The token after the whole statement
 * returns:
 *   StatementNode*          The statement to assemble.
 */
StatementNode *parse_if_statement (void) {

  /* local variables */
  char *uccontent, /* contents of the THEN token in upper case */
    *ucptr; /* pointer for upper case conversion */
  Token *token; /* tokens read as part of the statement */
  StatementNode *statement; /* the IF statement */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_IF;
  statement->statement.ifn = statement_create_if ();

  /* parse the first expression */
  statement->statement.ifn->left = parse_expression ();

  /* parse the operator */
  if (! errors_get_code ()) {
    token = get_token_to_parse ();
    if (token->class != TOKEN_SYMBOL)
      errors_set_code (E_INVALID_OPERATOR, token->line, last_label);
    else if (! strcmp (token->content, "="))
      statement->statement.ifn->op = RELOP_EQUAL;
    else if (! strcmp (token->content, "<>")
      || ! strcmp (token->content, "><"))
      statement->statement.ifn->op = RELOP_UNEQUAL;
    else if (! strcmp (token->content, "<"))
      statement->statement.ifn->op = RELOP_LESSTHAN;
    else if (! strcmp (token->content, "<="))
      statement->statement.ifn->op = RELOP_LESSOREQUAL;
    else if (! strcmp (token->content, ">"))
      statement->statement.ifn->op = RELOP_GREATERTHAN;
    else if (! strcmp (token->content, ">="))
      statement->statement.ifn->op = RELOP_GREATEROREQUAL;
    else
      errors_set_code (E_INVALID_OPERATOR, token->line, last_label);
  }

  /* parse the second expression */
  if (! errors_get_code ())
    statement->statement.ifn->right = parse_expression ();

  /* parse the THEN */
  if (! errors_get_code ()) {
    token = get_token_to_parse ();
    if (token->class != TOKEN_WORD)
      errors_set_code (E_THEN_EXPECTED, token->line, last_label);
    else {
      uccontent = malloc (strlen (token->content) + 1);
      strcpy (uccontent, token->content);
      for (ucptr = uccontent; *ucptr; ++ucptr)
        *ucptr = toupper (*ucptr);
      if (tinybasic_strcmp (uccontent, "THEN"))
        errors_set_code (E_THEN_EXPECTED, token->line, last_label);
      free (uccontent);
    }
  }

  /* parse the conditional statement */
  if (! errors_get_code ())
    statement->statement.ifn->statement = parse_statement ();

  /* destroy the half-made statement if errors occurred */
  if (errors_get_code ()) {
    statement_destroy (statement);
    statement = NULL;
  }

  /* return the statement */
  return statement;
}

/*
 * Parse a GOTO statement
 * returns:
 *   StatementNode*   the parsed GOTO statement
 */
StatementNode *parse_goto_statement (void) {

  /* local variables */
  StatementNode *statement; /* the IF statement */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_GOTO;
  statement->statement.goton = statement_create_goto ();

  /* parse the line label expression */
  if (! (statement->statement.goton->label = parse_expression ())) {
    statement_destroy (statement);
    statement = NULL;
  }

  /* return the new statement */
  return statement;
}

/*
 * Parse a GOSUB statement
 * returns:
 *   StatementNode*   the parsed GOSUB statement
 */
StatementNode *parse_gosub_statement (void) {

  /* local variables */
  StatementNode *statement; /* the IF statement */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_GOSUB;
  statement->statement.gosubn = statement_create_gosub ();

  /* parse the line label expression */
  if (! (statement->statement.gosubn->label = parse_expression ())) {
    statement_destroy (statement);
    statement = NULL;
  }

  /* return the new statement */
  return statement;
}

/*
 * Parse an RETURN statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_return_statement (void) {
  StatementNode *statement; /* the RETURN */
  statement = statement_create ();
  statement->class = STATEMENT_RETURN;
  return statement;
}

/*
 * Parse an END statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_end_statement (void) {
  StatementNode *statement = NULL; /* the END */
  statement = statement_create ();
  statement->class = STATEMENT_END;
  return statement;
}

/*
 * Parse a PRINT statement
 * globals:
 *   Token*   stored_token   The token after the whole statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_print_statement (void) {

  /* local variables */
  Token *token; /* tokens read as part of the statement */
  StatementNode *statement; /* the statement we're building */
  int line; /* line containing the PRINT token */
  OutputNode
    *nextoutput = NULL, /* the next output node we're parsing */
    *lastoutput = NULL; /* the last output node we parsed */
  ExpressionNode *expression; /* a parsed expression */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_PRINT;
  statement->statement.printn = statement_create_print ();
  line = tokeniser_get_line ();

  /* main loop for parsing the output list */
  do {
    token = get_token_to_parse ();

    /* process a premature end of line */
    if (token->class == TOKEN_EOF || token->line != line) {
      errors_set_code (E_INVALID_PRINT_OUTPUT, line, last_label);
      statement_destroy (statement);
      statement = NULL;
    }

    /* process a literal string */
    else if (token->class == TOKEN_STRING) {
      nextoutput = malloc (sizeof (OutputNode));
      nextoutput->class = OUTPUT_STRING;
      nextoutput->output.string = malloc (1 + strlen (token->content));
      strcpy (nextoutput->output.string, token->content);
      nextoutput->next = NULL;
      token_destroy (token);
    }

    /* attempt to process an expression */
    else {
      stored_token = token;
      if ((expression = parse_expression ())) {
        nextoutput = malloc (sizeof (OutputNode));
        nextoutput->class = OUTPUT_EXPRESSION;
        nextoutput->output.expression = expression;
        nextoutput->next = NULL;
      } else {
        errors_set_code (E_INVALID_PRINT_OUTPUT, token->line, 
          last_label);
        statement_destroy (statement);
        statement = NULL;
      }
    }

    /* add this output item to the statement and look for another */
    if (! errors_get_code ()) {
      if (lastoutput)
        lastoutput->next = nextoutput;
      else
        statement->statement.printn->first = nextoutput;
      lastoutput = nextoutput;
      token = get_token_to_parse ();
    }

  /* continue the loop until the statement appears to be finished */
  } while (! errors_get_code ()
    && token->class == TOKEN_SYMBOL
    && ! strcmp (token->content, ","));

  /* push back the last token and return the assembled statement */
  stored_token = token;
  return statement;
}

/*
 * Parse an INPUT statement
 * globals:
 *   Token*   stored_token   The token after the whole statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_input_statement (void) {

  /* local variables */
  Token *token; /* tokens read as part of the statement */
  StatementNode *statement; /* the statement we're building */
  int line; /* line containing the INPUT token */
  VariableListNode
    *nextvar = NULL, /* the next variable node we're parsing */
    *lastvar = NULL; /* the last variable node we parsed */

  /* initialise the statement */
  statement = statement_create ();
  statement->class = STATEMENT_INPUT;
  statement->statement.inputn = statement_create_input ();
  line = tokeniser_get_line ();

  /* main loop for parsing the variable list */
  do {
    token = get_token_to_parse ();

    /* process a premature end of line */
    if (token->class == TOKEN_EOF || token->line != line) {
      errors_set_code (E_INVALID_VARIABLE, line, last_label);
      statement_destroy (statement);
      statement = NULL;
    }

    /* attempt to process an variable name */
    else if (token->class != TOKEN_WORD) {
      errors_set_code (E_INVALID_VARIABLE, token->line, last_label);
      statement_destroy (statement);
      statement = NULL;
    } else if (strlen (token->content) != 1) {
      errors_set_code (E_INVALID_VARIABLE, token->line, last_label);
      statement_destroy (statement);
      statement = NULL;
    } else if (toupper (*token->content) < 'A'
      || toupper (*token->content) > 'Z') {
      errors_set_code (E_INVALID_VARIABLE, token->line, last_label);
      statement_destroy (statement);
      statement = NULL;
    } else {
      nextvar = malloc (sizeof (VariableListNode));
      nextvar->variable = toupper(*token->content) - 'A' + 1;
      nextvar->next = NULL;
    }

    /* add this variable to the statement and look for another */
    if (! errors_get_code ()) {
      if (lastvar)
        lastvar->next = nextvar;
      else
        statement->statement.inputn->first = nextvar;
      lastvar = nextvar;
      token = get_token_to_parse ();
    }
  } while (! errors_get_code ()
    && token->class == TOKEN_SYMBOL
    && ! strcmp (token->content, ","));

  /* return the assembled statement */
  stored_token = token;
  return statement;
}


/*
 * Level ? Routines
 */


/*
 * Parse a statement from the source file
 * globals:
 *   Token             *stored_token   a token already pre-read.
 * returns:
 *   StatementNode                     a fully-assembled statement, hopefully.
 */
StatementNode *parse_statement () {

  /* local variables */
  Token *token; /* token read */
  StatementNode *statement = NULL; /* the new statement */
  StatementClass class; /* class of command keyword read */

  /* get the next token */
  token = get_token_to_parse ();

  /* check for command */
  switch ((class = get_statement_class (token))) {
    case STATEMENT_NONE:
      stored_token = token;
      statement = NULL;
      break;
    case STATEMENT_LET:
      token_destroy (token);
      statement = parse_let_statement ();
      break;
    case STATEMENT_IF:
      token_destroy (token);
      statement = parse_if_statement ();
      break;
    case STATEMENT_GOTO:
      token_destroy (token);
      statement = parse_goto_statement ();
      break;
    case STATEMENT_GOSUB:
      token_destroy (token);
      statement = parse_gosub_statement ();
      break;
    case STATEMENT_END:
      token_destroy (token);
      statement = parse_end_statement ();
      break;
    case STATEMENT_PRINT:
      token_destroy (token);
      statement = parse_print_statement ();
      break;
    case STATEMENT_INPUT:
      token_destroy (token);
      statement = parse_input_statement ();
      break;
    case STATEMENT_RETURN:
      token_destroy (token);
      statement = parse_return_statement ();
      break;
    default:
      errors_set_code (E_UNRECOGNISED_COMMAND, token->line, last_label);
      token_destroy (token);
  }

  /* return the statement */
  return statement;
}


/*
 * Level ? Routines
 */


/*
 * Parse a line from the source file.
 * globals:
 *   Token   *stored_token   a token already pre-read.
 *   int     last_label      last line label used.
 *   int     current_line    the source line we're parsing
 * returns:
 *   StatementNode           a fully-assembled statement, hopefully.
 */
ProgramLineNode *parse_program_line (FILE *fh) {

  /* local variables */
  Token *token; /* token read */
  ProgramLineNode *program_line; /* program line read */

  /* initialise the program line and get the first token */
  input = fh;
  program_line = program_line_create ();
  program_line->label = generate_default_label ();
  token = get_token_to_parse ();

  /* deal with end of file */
  if (token->class == TOKEN_EOF) {
    token_destroy (token);
    program_line_destroy (program_line);
    return NULL;
  }

  /* deal with line label, if supplied */
  if (token->class == TOKEN_NUMBER) {
    sscanf(token->content, "%d", &program_line->label);
    token_destroy (token);
  } else
    stored_token = token;

  /* validate the supplied or implied line label */
  if (validate_line_label (program_line->label))
    last_label = program_line->label;
  else {
    errors_set_code (E_INVALID_LINE_NUMBER, current_line, last_label);
    program_line_destroy (program_line);
    return NULL;
  }

  /* check for a statement and an EOL */
  program_line->statement = parse_statement ();
  token = get_token_to_parse ();
  if (token->class != TOKEN_EOL && token->class != TOKEN_EOF)
    errors_set_code (E_UNEXPECTED_PARAMETER, current_line, last_label);

  /* return the program line */
  return program_line;
}


/*
 * Top Level Parser Routines
 */


/*
 * Parse the whole program
 * params:
 *   FILE*   input   the input file
 * returns:
 *   ProgramNode*    a pointer to the whole program
 */
ProgramNode *parse_program (FILE *input) {

  /* local varables */
  ProgramNode *program; /* the stored program */
  ProgramLineNode
    *previous = NULL, /* the previous line */
    *current; /* the current line */

  /* initialise the program */
  program = malloc (sizeof (ProgramNode));
  program->first = NULL;

  /* read lines until reaching an error or end of input */
  while ((current = parse_program_line (input))
    && ! errors_get_code ()) {
    if (previous)
      previous->next = current;
    else
      program->first = current;
    previous = current;
  }

  /* return the program */
  return program;
}

/*
 * Return the current source line we're parsing
 * globals:
 *   int   current_line   the line stored when the last token was read
 * returns:
 *   int                  the line returned
 */
int parser_line (void) {
  return current_line;
}

/*
 * Return the label of the source line we're parsing
 * globals:
 *   int   last_label   the label stored when validated
 * returns:
 *   int                the label returned
 */
int parser_label (void) {
  return last_label;
}
