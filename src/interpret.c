/*
 * Tiny BASIC Interpreter and Compiler Project
 * Interpreter module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 23-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "statement.h"


/* global variables */
static ProgramNode *stored_program; /* a global copy of the program */
static int variables[26]; /* the numeric variables */


/* forward declarations */
int interpret_expression (ExpressionNode *expression);


/*
 * Level 4 Routines
 */


/*
 * Evaluate a factor for the interpreter
 * params:
 *   FactorNode*   factor   the factor to evaluate
 */
int interpret_factor (FactorNode *factor) {
  switch (factor->class) {
    case FACTOR_VARIABLE:
      return variables[factor->data.variable - 1];
    case FACTOR_VALUE:
      return factor->data.value;
    case FACTOR_EXPRESSION:
      return interpret_expression (factor->data.expression);
    default:
      break;
  }
  return 0; /* needed for brain dead compiler */
}


/*
 * Level 3 Routines
 */


/*
 * Evaluate a term for the interpreter
 * params:
 *   TermNode*   term   the term to evaluate
 */
int interpret_term (TermNode *term) {

  /* local variables */
  int result_store; /* the partial evaluation */
  RightHandFactor *rhfactor; /* pointer to successive rh factor nodes */

  /* calculate the first factor result */
  result_store = interpret_factor (term->factor);
  rhfactor = term->next;

  /* adjust store according to successive rh factors */
  while (rhfactor) {
    switch (rhfactor->op) {
      case TERM_OPERATOR_MULTIPLY:
        result_store *= interpret_factor (rhfactor->factor);
        break;
      case TERM_OPERATOR_DIVIDE:
        result_store /= interpret_factor (rhfactor->factor);
        break;
      default:
        break;
    }
    rhfactor = rhfactor->next;
  }

  /* return the result */
  return result_store;
}


/*
 * Level 2 Routines
 */


/*
 * Evaluate an expression for the interpreter
 * params:
 *   ExpressionNode*   expression   the expression to evaluate
 */
int interpret_expression (ExpressionNode *expression) {

  /* local variables */
  int result_store; /* the partial evaluation */
  RightHandTerm *rhterm; /* pointer to successive rh term nodes */

  /* calculate the first term result */
  result_store = interpret_term (expression->term);
  rhterm = expression->next;

  /* adjust store according to successive rh terms */
  while (rhterm) {
    switch (rhterm->op) {
      case EXPRESSION_OPERATOR_PLUS:
        result_store += interpret_term (rhterm->term);
        break;
      case EXPRESSION_OPERATOR_MINUS:
        result_store -= interpret_term (rhterm->term);
        break;
      default:
        break;
    }
    rhterm = rhterm->next;
  }

  /* return the result */
  return result_store;
}


/*
 * Level 1 Routines
 */


/*
 * Initialise the Variables
 * globals:
 *   int[]   variables   the program's variables
 */
void interpret_initialise_variables (void) {
  int count; /* counter for variables */
  for (count = 0; count < 26; ++count) {
    variables[count] = 0;
  }
}

/*
 * Interpret a LET statement
 * params:
 *   LetStatementNode*   letn   the LET statement details
 */
void interpret_let_statement (LetStatementNode *letn) {
  variables[letn->variable - 1] = interpret_expression (letn->expression);
}

/*
 * Interpret a PRINT statement
 * params:
 *   PrintStatementNode*   printn   the PRINT statement details
 */
void interpret_print_statement (PrintStatementNode *printn) {

  /* local variables */
  OutputNode *outn; /* current output node */

  /* print each of the output items */
  outn = printn->first;
  while (outn) {
    switch (outn->class) {
      case OUTPUT_STRING:
        printf ("%s", outn->output.string);
        break;
      case OUTPUT_EXPRESSION:
        printf ("%d", interpret_expression (outn->output.expression));
        break;
    }
    outn = outn->next;
  }

  /* print the linefeed */
  printf ("\n");
}

/*
 * Interpret program starting from a particular statement
 * params:
 *   StatementNode*   statement   the starting statement
 */
void interpret_program_from (StatementNode *statement) {

  /* local variables */
  StatementNode *current; /* statement we're executing now */

  /* main loop */
  current = statement;
  while (current) {
    switch (current->class) {

      /* interpret a LET */
      case STATEMENT_LET:
        interpret_let_statement (current->statement.letn);
        current = current->next;
        break;

      /* interpret a PRINT */
      case STATEMENT_PRINT:
        interpret_print_statement (current->statement.printn);
        current = current->next;
        break;

      /* unknown statement type reached */
      default:
        printf ("Statement type %d not implemented.\n", current->class);
        current = NULL;
    }
  }
}


/*
 * Top Level Routines
 */


/*
 * Interpret the program from the beginning
 * params:
 *   ProgramNode*   program   the program to interpret
 */
void interpret_program (ProgramNode *program) {
  stored_program = program;
  interpret_initialise_variables ();
  interpret_program_from (program->first);
}