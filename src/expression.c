/*
 * Tiny BASIC
 * Expression Handling Module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 16-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"
#include "errors.h"


/*
 * Internal Function Declarations
 */


/* factor_output() has a forward reference to expression_output() */
char *expression_output (ExpressionNode *expression);


/*
 * Functions for Dealing with Factors
 */


/*
 * Constructor for a factor
 * returns:
 *   FactorNode*   the new factor
 */
FactorNode *factor_create (void) {

  /* local variables */
  FactorNode *factor; /* the new factor */

  /* allocate memory and initialise members */
  factor = malloc (sizeof (FactorNode));
  factor->class = FACTOR_NONE;
  factor->sign = SIGN_POSITIVE;

  /* return the factor */
  return factor;
}

/*
 * Destructor for a factor
 * params:
 *   FactorNode*   factor   the doomed factor
 */
void factor_destroy (FactorNode *factor) {
  if (factor->class == FACTOR_EXPRESSION && factor->data.expression) {
    expression_destroy (factor->data.expression);
  }
  free (factor);
}

/*
 * Output a factor
 * params:
 *   FactorNode*   factor   the factor to output
 * return:
 *   char*                  the text representation of the factor
 */
char *factor_output (FactorNode *factor) {

  /* local variables */
  char *factor_text = NULL, /* the text of the whole factor */
    *factor_buffer = NULL, /* temporary buffer for prepending to factor_text */
    *expression_text = NULL; /* the text of a subexpression */

  /* work out the main factor text */
  switch (factor->class) {
    case FACTOR_VARIABLE:
      factor_text = malloc (2);
      sprintf (factor_text, "%c", factor->data.variable + 'A' - 1);
      break;
    case FACTOR_VALUE:
      factor_text = malloc (7);
      sprintf (factor_text, "%d", factor->data.value);
      break;
    case FACTOR_EXPRESSION:
      if ((expression_text = expression_output (factor->data.expression))) {
        factor_text = malloc (strlen (expression_text) + 3);
        sprintf (factor_text, "(%s)", expression_text);
        free (expression_text);
      }
      break;
    default:
      errors_set_code (E_INVALID_EXPRESSION);
  }

  /* apply a negative sign, if necessary */
  if (factor_text && factor->sign == SIGN_NEGATIVE) {
    factor_buffer = malloc (strlen (factor_text) + 2);
    sprintf (factor_buffer, "-%s", factor_text);
    free (factor_text);
    factor_text = factor_buffer;
  }

  /* return the final factor representation */
  return factor_text;
}


/*
 * Functions for Dealing with Terms
 */


/*
 * Constructor for a right-hand factor of a term
 * returns:
 *   RightHandFactor*   the new RH factor of a term
 */
RightHandFactor *rhfactor_create (void) {

  /* local variables */
  RightHandFactor *rhfactor; /* the RH factor of a term to create */

  /* allocate memory and initialise members */
  rhfactor = malloc (sizeof (RightHandFactor));
  rhfactor->op = TERM_OPERATOR_NONE;
  rhfactor->factor = NULL;
  rhfactor->next = NULL;

  /* return the new RH term */
  return rhfactor;
}

/*
 * Recursive destructor for a right-hand factor of a term
 * params:
 *   RightHandFactor*   rhfactor   the doomed RH factor of a term
 */
void rhfactor_destroy (RightHandFactor *rhfactor) {
  if (rhfactor->next)
    rhfactor_destroy (rhfactor->next);
  if (rhfactor->factor)
    factor_destroy (rhfactor->factor);
  free (rhfactor);
}

/*
 * Constructor for a term
 * returns:
 *   TermNode*   the new term
 */
TermNode *term_create (void) {

  /* local variables */
  TermNode *term; /* the new term */

  /* allocate memory and initialise members */
  term = malloc (sizeof (TermNode));
  term->factor = NULL;
  term->next = NULL;

  /* return the new term */
  return term;
}

/*
 * Destructor for a term
 * params:
 *   TermNode*   term   the doomed term
 */
void term_destroy (TermNode *term) {

  /* destroy the consituent parts of the term */
  if (term->factor)
    factor_destroy (term->factor);
  if (term->next)
    rhfactor_destroy (term->next);

  /* destroy the term itself */
  free (term);
}

/*
 * Output a term
 * params:
 *   TermNode*   term   the term to output
 * returns:
 *   char*              the text representation of the term
 */
char *term_output (TermNode *term) {

  /* local variables */
  char
    *term_text = NULL, /* the text of the whole term */
    *factor_text = NULL, /* the text of each factor */
    operator_char; /* the operator that joins the righthand factor */
  RightHandFactor *rhfactor; /* right hand factors of the expression */

  /* begin with the initial factor */
  if ((term_text = factor_output (term->factor))) {
    rhfactor = term->next;
    while (! errors_get_code () && rhfactor) {

      /* ascertain the operator text */
      switch (rhfactor->op) {
      case TERM_OPERATOR_MULTIPLY:
        operator_char = '*';
        break;
      case TERM_OPERATOR_DIVIDE:
        operator_char = '/';
        break;
      default:
        errors_set_code (E_INVALID_EXPRESSION);
        free (term_text);
        term_text = NULL;
      }

      /* get the factor that follows the operator */
      if (! errors_get_code ()
        && (factor_text = factor_output (rhfactor->factor))) {
        term_text = realloc (term_text,
          strlen (term_text) + strlen (factor_text) + 2);
        sprintf (term_text, "%s%c%s", term_text, operator_char, factor_text);
        free (factor_text);
      }

      /* look for another term on the right of the expression */
      rhfactor = rhfactor->next;
    }
  }

  /* return the expression text */
  return term_text;

}


/*
 * Functions for dealing with Expressions
 */


/*
 * Constructor for a right-hand expression
 * returns:
 *   RightHandTerm*   the RH term of an expression
 */
RightHandTerm *rhterm_create (void) {

  /* local variables */
  RightHandTerm *rhterm; /* the new RH expression */

  /* allocate memory and initialise members */
  rhterm = malloc (sizeof (RightHandTerm));
  rhterm->op = EXPRESSION_OPERATOR_NONE;
  rhterm->term = NULL;
  rhterm->next = NULL;

  /* return the new right-hand expression */
  return rhterm;
}

/*
 * Recursive destructor for a right-hand term of an expression
 * params:
 *   RightHandTerm*   rhterm   the doomed RH expression
 */
void rhterm_destroy (RightHandTerm *rhterm) {
  if (rhterm->next)
    rhterm_destroy (rhterm->next);
  if (rhterm->term)
    term_destroy (rhterm->term);
  free (rhterm);
}

/*
 * Constructor for an expression
 * returns:
 *   ExpressionNode*   the new expression
 */
ExpressionNode *expression_create (void) {

  /* local variables */
  ExpressionNode *expression; /* the new expression */

  /* allocate memory and initialise members */
  expression = malloc (sizeof (ExpressionNode));
  expression->term = NULL;
  expression->next = NULL;

  /* return the new expression */
  return expression;
}

/*
 * Destructor for a expression
 * params:
 *   ExpressionNode*   expression   the doomed expression
 */
void expression_destroy (ExpressionNode *expression) {

  /* destroy the consituent parts of the expression */
  if (expression->term)
    term_destroy (expression->term);
  if (expression->next)
    rhterm_destroy (expression->next);

  /* destroy the expression itself */
  free (expression);
}

/*
 * Output an expression for a program listing
 * params:
 *   ExpressionNode*   expression   the expression to output
 * returns:
 *   char*                          new string containint the expression text
 */
char *expression_output (ExpressionNode *expression) {

  /* local variables */
  char
    *expression_text = NULL, /* the text of the whole expression */
    *term_text = NULL, /* the text of each term */
    operator_char; /* the operator that joins the righthand term */
  RightHandTerm *rhterm; /* right hand terms of the expression */

  /* begin with the initial term */
  if ((expression_text = term_output (expression->term))) {
    rhterm = expression->next;
    while (! errors_get_code () && rhterm) {

      /* ascertain the operator text */
      switch (rhterm->op) {
      case EXPRESSION_OPERATOR_PLUS:
        operator_char = '+';
        break;
      case EXPRESSION_OPERATOR_MINUS:
        operator_char = '-';
        break;
      default:
        errors_set_code (E_INVALID_EXPRESSION);
        free (expression_text);
        expression_text = NULL;
      }

      /* get the terms that follow the operators */
      if (! errors_get_code ()
        && (term_text = term_output (rhterm->term))) {
        expression_text = realloc (expression_text,
          strlen (expression_text) + strlen (term_text) + 2);
        sprintf (expression_text, "%s%c%s", expression_text, operator_char,
          term_text);
        free (term_text);
      }

      /* look for another term on the right of the expression */
      rhterm = rhterm->next;
    }
  }

  /* return the expression text */
  return expression_text;

}
