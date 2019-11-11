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
#include "options.h"
#include "statement.h"


/* forward declarations */
int interpret_expression (ExpressionNode *expression);
void interpret_statement (StatementNode *statement);


/*
 * Data Definitions
 */

/* The GOSUB Stack */
typedef struct gosub_stack_node GosubStackNode;
typedef struct gosub_stack_node {
  ProgramLineNode *program_line; /* the line following the GOSUB */
  GosubStackNode *next; /* stack node for the previous GOSUB */
} GosubStackNode;


/* global variables */
static ProgramNode *stored_program; /* a global copy of the program */
static ProgramLineNode *current_line; /* current line we're executing */
static GosubStackNode *gosub_stack = NULL; /* the top of the GOSUB stack */
static int variables[26]; /* the numeric variables */
static int run_ended = 0; /* set to 1 when an END is encountered */
static ErrorHandler *errors; /* the error handler */


/*
 * Level 4 Routines
 */


/*
 * Evaluate a factor for the interpreter
 * params:
 *   FactorNode*   factor   the factor to evaluate
 */
int interpret_factor (FactorNode *factor) {

  /* local variables */
  int result_store = 0; /* result of factor evaluation */

  /* check factor class */
  switch (factor->class) {

    /* a regular variable */
    case FACTOR_VARIABLE:
      result_store = variables[factor->data.variable - 1]
        * (factor->sign == SIGN_POSITIVE ? 1 : -1);
      break;

    /* an integer constant */
    case FACTOR_VALUE:
      result_store = factor->data.value * (factor->sign == SIGN_POSITIVE
	? 1 : -1);
      break;

    /* an expression */
    case FACTOR_EXPRESSION:
      result_store = interpret_expression (factor->data.expression)
        * (factor->sign == SIGN_POSITIVE ? 1 : -1);
      break;

    /* this only happens if the parser has failed in its duty */
    default:
      errors->set_code (errors, E_INVALID_EXPRESSION, 0, current_line->label);
  }

  /* check the result and return it*/
  if (result_store < -32768 || result_store > 32767)
    errors->set_code (errors, E_OVERFLOW, 0, current_line->label);
  return result_store;
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
  int divisor; /* used to check for division by 0 before attempting */

  /* calculate the first factor result */
  result_store = interpret_factor (term->factor);
  rhfactor = term->next;

  /* adjust store according to successive rh factors */
  while (rhfactor && ! errors->get_code (errors)) {
    switch (rhfactor->op) {
      case TERM_OPERATOR_MULTIPLY:
        result_store *= interpret_factor (rhfactor->factor);
	if (result_store < -32768 || result_store > 32767)
	  errors->set_code (errors, E_OVERFLOW, 0, current_line->label);
        break;
      case TERM_OPERATOR_DIVIDE:
        if ((divisor = interpret_factor (rhfactor->factor)))
          result_store /= divisor;
        else
          errors->set_code (errors, E_DIVIDE_BY_ZERO, 0, current_line->label);
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
  while (rhterm && ! errors->get_code (errors)) {
    switch (rhterm->op) {
      case EXPRESSION_OPERATOR_PLUS:
        result_store += interpret_term (rhterm->term);
	if (result_store < -32768 || result_store > 32767)
	  errors->set_code (errors, E_OVERFLOW, 0, current_line->label);
        break;
      case EXPRESSION_OPERATOR_MINUS:
        result_store -= interpret_term (rhterm->term);
	if (result_store < -32768 || result_store > 32767)
	  errors->set_code (errors, E_OVERFLOW, 0, current_line->label);
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
 * Find a program line given its label
 * returns:
 *   ProgramLineNode*   the program line found
 */
ProgramLineNode *interpret_label_search (int jump_label) {

  /* local variables */
  ProgramLineNode
    *ptr, /* a line we're currently looking at */
    *found = NULL; /* the line if found */

  /* do the search */
  for (ptr = stored_program->first; ptr && ! found; ptr = ptr->next)
    if (ptr->label == jump_label)
      found = ptr;
    else if (ptr->label >= jump_label
      && options_get ().line_numbers != LINE_NUMBERS_OPTIONAL)
      found = ptr;

  /* check for errors and return what was found */
  if (! found)
    errors->set_code (errors, E_INVALID_LINE_NUMBER, 0, current_line->label);
  return found;
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
  current_line = current_line->next;
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
  if (comparison && ! errors->get_code (errors))
    interpret_statement (ifn->statement);
  else
    current_line = current_line->next;
}

/*
 * Interpret a GOTO statement
 * params:
 *   GotoStatementNode*   goton   the GOTO statement details
 */
void interpret_goto_statement (GotoStatementNode *goton) {
  int label; /* the line label to go to */
  label = interpret_expression (goton->label);
  if (! errors->get_code (errors))
    current_line = interpret_label_search (label);
}

/*
 * Interpret a GOSUB statement
 * params:
 *   GosubStatementNode*   gosubn   the GOSUB statement details
 */
void interpret_gosub_statement (GosubStatementNode *gosubn) {

  /* local variables */
  GosubStackNode *gosub_node; /* indicates the program line to return to */
  int label; /* the line label to go to */

  /* create the new node on the GOSUB stack */
  gosub_node = malloc (sizeof (GosubStackNode));
  gosub_node->program_line = current_line->next;
  gosub_node->next = gosub_stack;
  gosub_stack = gosub_node;

  /* branch to the subroutine requested */
  label = interpret_expression (gosubn->label);
  if (! errors->get_code (errors))
    current_line = interpret_label_search (label);
}

/*
 * Interpret a RETURN statement
 */
void interpret_return_statement (void) {

  /* local variables */
  GosubStackNode *gosub_node; /* node popped off the GOSUB stack */

  /* return to the statement following the most recent GOSUB */
  if (gosub_stack) {
    current_line = gosub_stack->program_line;
    gosub_node = gosub_stack;
    gosub_stack = gosub_stack->next;
    free (gosub_node);
  }

  /* no GOSUBs led here, so raise an error */
  else
    errors->set_code (errors, E_RETURN_WITHOUT_GOSUB, 0, current_line->label);
}

/*
 * Interpret a PRINT statement
 * params:
 *   PrintStatementNode*   printn   the PRINT statement details
 */
void interpret_print_statement (PrintStatementNode *printn) {

  /* local variables */
  OutputNode *outn; /* current output node */
  int
    items = 0, /* counter ensures runtime errors appear on a new line */
    result; /* the result of an expression */

  /* print each of the output items */
  outn = printn->first;
  while (outn) {
    switch (outn->class) {
      case OUTPUT_STRING:
        printf ("%s", outn->output.string);
        ++items;
        break;
      case OUTPUT_EXPRESSION:
        result = interpret_expression (outn->output.expression);
        if (! errors->get_code (errors)) {
          printf ("%d", result);
          ++items;
        }
        break;
    }
    outn = outn->next;
  }

  /* print the linefeed */
  if (items)
    printf ("\n");
  current_line = current_line->next;
}

/*
 * Interpret an INPUT statement
 * params:
 *   InputStatementNode*   inputn   the INPUT statement details
 */
void interpret_input_statement (InputStatementNode *inputn) {

  /* local variables */
  VariableListNode *variable; /* current variable to input */
  int
    value, /* value input from the user */
    sign = 1, /* the default sign */
    ch = 0; /* character from the input stream */

  /* input each of the variables */
  variable = inputn->first;
  while (variable) {
    do {
      if (ch == '-') sign = -1; else sign = 1;
      ch = getchar ();
    } while (ch < '0' || ch > '9');
    value = 0;
    do {
      value = 10 * value + (ch - '0');
      if (value * sign < -32768 || value * sign > 32767)
	errors->set_code (errors, E_OVERFLOW, 0, current_line->label);
      ch = getchar ();
    } while (ch >= '0' && ch <= '9' && ! errors->get_code (errors));
    variables[variable->variable - 1] = sign * value;
    variable = variable->next;
  }

  /* advance to the next statement when done */
  current_line = current_line->next;
}


/*
 * Interpret an individual statement
 * params:
 *   StatementNode*   statement   the statement to interpret
 */
void interpret_statement (StatementNode *statement) {

  /* skip comments */
  if (! statement) {
    current_line = current_line->next;
    return;
  }

  /* interpret real statements */
  switch (statement->class) {
    case STATEMENT_NONE:
      break;
    case STATEMENT_LET:
      interpret_let_statement (statement->statement.letn);
      break;
    case STATEMENT_IF:
      interpret_if_statement (statement->statement.ifn);
      break;
    case STATEMENT_GOTO:
      interpret_goto_statement (statement->statement.goton);
      break;
    case STATEMENT_GOSUB:
      interpret_gosub_statement (statement->statement.gosubn);
      break;
    case STATEMENT_RETURN:
      interpret_return_statement ();
      break;
    case STATEMENT_END:
       run_ended = 1;
     break;
    case STATEMENT_PRINT:
      interpret_print_statement (statement->statement.printn);
      break;
    case STATEMENT_INPUT:
      interpret_input_statement (statement->statement.inputn);
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
  current_line = program_line;
  while (current_line && ! run_ended && ! errors->get_code (errors))
    interpret_statement (current_line->statement);
}


/*
 * Top Level Routines
 */


/*
 * Interpret the program from the beginning
 * params:
 *   ProgramNode*    program          the program to interpret
 *   ErrorHandler*   runtime_errors   runtime error handler
 */
void interpret_program (ProgramNode *program, ErrorHandler *runtime_errors) {
  stored_program = program;
  errors = runtime_errors;
  interpret_initialise_variables ();
  interpret_program_from (program->first);
}
