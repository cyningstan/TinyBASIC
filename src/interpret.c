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


/* forward declarations */
int interpret_expression (ExpressionNode *expression);
void interpret_statement (StatementNode *statement);


/* global variables */
static ProgramNode *stored_program; /* a global copy of the program */
static int variables[26]; /* the numeric variables */
static int jump_label; /* destination of a GOTO or GOSUB */


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
 * Interpret an IF statement
 * params:
 *   IfStatementNode*   ifn   the IF statement details
 */
void interpret_if_statement (IfStatementNode *ifn) {

  /* local variables */
  int
    left, /* result of the left-hand expression */
    right, /* result of the right-hand expression */
    comparison; /* result of the comparison between the two */

  /* get the expressions */
  left = interpret_expression (ifn->left);
  right = interpret_expression (ifn->right);

  /* make the comparison */
  switch (ifn->op) {
    case RELOP_EQUAL: comparison = (left == right); break;
    case RELOP_UNEQUAL: comparison = (left != right); break;
    case RELOP_LESSTHAN: comparison = (left < right); break;
    case RELOP_LESSOREQUAL: comparison = (left <= right); break;
    case RELOP_GREATERTHAN: comparison = (left > right); break;
    case RELOP_GREATEROREQUAL: comparison = (left >= right); break;
  }

  /* perform the conditional statement */
  if (comparison)
    interpret_statement (ifn->statement);
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
 * Interpret an individual statement
 * params:
 *   StatementNode*   statement   the statement to interpret
 */
void interpret_statement (StatementNode *statement) {
  switch (statement->class) {
    case STATEMENT_LET:
      interpret_let_statement (statement->statement.letn);
      break;
    case STATEMENT_IF:
      interpret_if_statement (statement->statement.ifn);
      break;
    case STATEMENT_PRINT:
      interpret_print_statement (statement->statement.printn);
      break;
    default:
      printf ("Statement type %d not implemented.\n", statement->class);
  }
}

/*
 * Interpret program starting from a particular line
 * params:
 *   ProgramLineNode*   program_line   the starting line
 */
void interpret_program_from (ProgramLineNode *program_line) {

  /* local variables */
  ProgramLineNode *current; /* line we're executing now */

  /* main loop */
  current = program_line;
  while (current) {
    interpret_statement (current->statement);
    switch (current->statement->class) {
      default:
        current = current->next;
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