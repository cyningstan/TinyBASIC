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

/* parse_statement() has a forward reference from parse_if_statement() */
StatementNode *parse_statement (void);

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

      /* set an error condition if we read a sign but not a factor */
      else {
        rhfactor_destroy (rhfactor);
        errors_set_code (E_INVALID_EXPRESSION);
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
 *   ExpressionNode*   a new expression node holding the parsed expression
 */
ExpressionNode *parse_expression (void) {

  /* local variables */
  ExpressionNode *expression = NULL; /* the expression we're building */
  TermNode *term; /* term detected */
  RightHandTerm
    *rhterm = NULL, /* the right-hand term detected */
    *rhptr = NULL; /* pointer to the previous right-hand term, if any */
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
        errors_set_code (E_INVALID_EXPRESSION);
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
    errors_set_code (E_INVALID_VARIABLE);
    statement_destroy (statement);
    return NULL;
  } else if (strlen (token->content) != 1) {
    errors_set_code (E_INVALID_VARIABLE);
    statement_destroy (statement);
    return NULL;
  } else if (toupper (*token->content) < 'A'
    || toupper (*token->content) > 'Z') {
    errors_set_code (E_INVALID_VARIABLE);
    statement_destroy (statement);
    return NULL;
  }
  statement->statement.letn->variable = toupper(*token->content) - 'A' + 1;

  /* get the "=" */
  token_destroy (token);
  token = get_token_to_parse ();
  if (token->class != TOKEN_SYMBOL) {
    errors_set_code (E_INVALID_ASSIGNMENT);
    statement_destroy (statement);
    return NULL;
  } else if (strcmp (token->content, "=")) {
    errors_set_code (E_INVALID_ASSIGNMENT);
    statement_destroy (statement);
    return NULL;
  }

  /* get the expression */
  statement->statement.letn->expression = parse_expression ();
  if (! statement->statement.letn->expression) {
    errors_set_code (E_INVALID_EXPRESSION);
    statement_destroy (statement);
    return NULL;
  }

  /* check for a line end */
  line = token->line;
  stored_token = token = get_token_to_parse ();
  if (token->class != TOKEN_EOF && token->line == line) {
    errors_set_code (E_INVALID_EXPRESSION);
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
      errors_set_code (E_INVALID_OPERATOR);
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
      errors_set_code (E_INVALID_OPERATOR);
  }

  /* parse the second expression */
  if (! errors_get_code ())
    statement->statement.ifn->right = parse_expression ();

  /* parse the THEN */
  if (! errors_get_code ()) {
    token = get_token_to_parse ();
    if (token->class != TOKEN_WORD)
      errors_set_code (E_THEN_EXPECTED);
    else {
      uccontent = malloc (strlen (token->content) + 1);
      strcpy (uccontent, token->content);
      for (ucptr = uccontent; *ucptr; ++ucptr)
        *ucptr = toupper (*ucptr);
      if (strcmp (uccontent, "THEN"))
        errors_set_code (E_THEN_EXPECTED);
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
 * globals:
 *   Token*   stored_token   The token after the RETURN statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_return_statement (void) {

  /* local variables */
  Token *token; /* token read to ensure the line contains only RETURN */
  StatementNode *statement = NULL; /* the RETURN */
  int line; /* line on which the RETURN token appeared */

  /* check that there's nothing on this line after the RETURN */
  line = tokeniser_get_line ();
  token = get_token_to_parse ();
  if (token->class == TOKEN_EOF || token->line > line) {
    statement = statement_create ();
    statement->class = STATEMENT_RETURN;
    stored_token = token;
  }

  /* if there is, raise an error and clean up the mess */
  else {
    errors_set_code (E_UNEXPECTED_PARAMETER);
    token_destroy (token);
  }

  /* return the statement */
  return statement;
}

/*
 * Parse an END statement
 * globals:
 *   Token*   stored_token   The token after the END statement
 * returns:
 *   StatementNode*          The statement assembled
 */
StatementNode *parse_end_statement (void) {

  /* local variables */
  Token *token; /* token read to ensure the line contains only END */
  StatementNode *statement = NULL; /* the END */
  int line; /* line on which the END token appeared */

  /* check that there's nothing on this line after the END */
  line = tokeniser_get_line ();
  token = get_token_to_parse ();
  if (token->class == TOKEN_EOF || token->line > line) {
    statement = statement_create ();
    statement->class = STATEMENT_END;
    stored_token = token;
  }

  /* if there is, raise an error and clean up the mess */
  else {
    errors_set_code (E_UNEXPECTED_PARAMETER);
    token_destroy (token);
  }

  /* return the statement */
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
  int
    line, /* line containing the PRINT token */
    print_done = 0; /* 1 when the end of the PRINT statement is reached */
  OutputNode
    *nextoutput = NULL, /* the next output node we're parsing */
    *lastoutput = NULL; /* the last output node we parsed */
  ExpressionNode *expression; /* a parsed expression in the output list */

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
      errors_set_code (E_INVALID_PRINT_OUTPUT);
      statement_destroy (statement);
      statement = NULL;
      print_done = 1;
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
        errors_set_code (E_INVALID_PRINT_OUTPUT);
        statement_destroy (statement);
        statement = NULL;
        print_done = 1;
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

      /* if we meet a comma, it means there's another output item */
      if (token->class == TOKEN_SYMBOL && ! strcmp (token->content, ","))
        continue;

      /* anything other than an EOL or EOF at this point is an error */
      if (token->class != TOKEN_EOF && token->line == line) {
        errors_set_code (E_INVALID_PRINT_OUTPUT);
        statement_destroy (statement);
        statement = NULL;
      }

      /* error or not, we're done with the PRINT statement now */
      print_done = 1;
      stored_token = token;
    }
  } while (! print_done);

  /* return the assembled statement */
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
  int
    line, /* line containing the INPUT token */
    input_done = 0; /* 1 when the end of the INPUT statement is reached */
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
      errors_set_code (E_INVALID_VARIABLE);
      statement_destroy (statement);
      statement = NULL;
      input_done = 1;
    }

    /* attempt to process an variable name */
    else if (token->class != TOKEN_WORD) {
      errors_set_code (E_INVALID_VARIABLE);
      statement_destroy (statement);
      statement = NULL;
    } else if (strlen (token->content) != 1) {
      errors_set_code (E_INVALID_VARIABLE);
      statement_destroy (statement);
      statement = NULL;
    } else if (toupper (*token->content) < 'A'
      || toupper (*token->content) > 'Z') {
      errors_set_code (E_INVALID_VARIABLE);
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

      /* if we meet a comma, it means there's another variable */
      if (token->class == TOKEN_SYMBOL && ! strcmp (token->content, ","))
        continue;

      /* anything other than an EOL or EOF at this point is an error */
      if (token->class != TOKEN_EOF && token->line == line) {
        errors_set_code (E_INVALID_VARIABLE);
        statement_destroy (statement);
        statement = NULL;
      }

      /* error or not, we're done with the PRINT statement now */
      input_done = 1;
      stored_token = token;
    }
  } while (! input_done);

  /* return the assembled statement */
  return statement;
}


/*
 * Level ? Routines
 */


/*
 * Parse an statment from the source file
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
    case STATEMENT_LET:
      statement = parse_let_statement ();
      break;
    case STATEMENT_IF:
      statement = parse_if_statement ();
      break;
    case STATEMENT_GOTO:
      statement = parse_goto_statement ();
      break;
    case STATEMENT_GOSUB:
      statement = parse_gosub_statement ();
      break;
    case STATEMENT_END:
      statement = parse_end_statement ();
      break;
    case STATEMENT_PRINT:
      statement = parse_print_statement ();
      break;
    case STATEMENT_INPUT:
      statement = parse_input_statement ();
      break;
    case STATEMENT_RETURN:
      statement = statement_create ();
      statement->class = class;
      _skip_line ();
      break;
    default:
      errors_set_code (E_UNRECOGNISED_COMMAND);
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
 *   Token             *stored_token   a token already pre-read.
 *   int               last_label      last line label used.
 * returns:
 *   StatementNode                     a fully-assembled statement, hopefully.
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
    errors_set_code (E_INVALID_LINE_NUMBER);
    program_line_destroy (program_line);
    return NULL;
  }

  /* check for command */
  program_line->statement = parse_statement ();

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