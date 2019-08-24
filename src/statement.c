/*
 * Tiny BASIC
 * Statement Handling Module
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 15-Aug-2019
 */


/* included headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "statement.h"


/*
 * Forward References
 */

/* statement_output() has a forward reference from statement_output_if() */
char *statement_output (StatementNode *statement);


/*
 * LET Statement Functions
 */


/*
 * LET statement constructor
 * returns:
 *   LetStatementNode*   the created LET statement
 */
LetStatementNode *statement_create_let (void) {

  /* local variables */
  LetStatementNode *letn; /* the created node */

  /* allocate memory and assign safe defaults */
  letn = malloc (sizeof (LetStatementNode));
  letn->variable = 0;
  letn->expression = NULL;

  /* return the LET statement node */
  return letn;
}

/*
 * Destructor for a LET statement
 * params:
 *   LetStatementNode   *letn   the doomed LET statement.
 */
void statement_destroy_let (LetStatementNode *letn) {
  if (letn->expression)
    expression_destroy (letn->expression);
  free (letn);
}

/*
 * LET statement output
 * params:
 *   LetStatementNode*   letn   data for the LET statement
 * returns:
 *   char*                      the LET statement text
 */
char *statement_output_let (LetStatementNode *letn) {

  /* local variables */
  char
    *let_text = NULL, /* the LET text to be assembled */
    *expression_text = NULL; /* the text of the expression */

  /* assemble the expression */
  expression_text = expression_output (letn->expression);

  /* assemble the final LET text, if we have an expression */
  if (expression_text) {
    let_text = malloc (7 + strlen (expression_text));
    sprintf (let_text, "LET %c=%s", 'A' - 1 + letn->variable, expression_text);
    free (expression_text);
  }

  /* return it */
  return let_text;
}


/*
 * IF Statement Functions
 */


/*
 * IF statement constructor
 * returns:
 *   IfStatementNode*   the created IF statement
 */
IfStatementNode *statement_create_if (void) {

  /* local variables */
  IfStatementNode *ifn; /* the created node */

  /* allocate memory and assign safe defaults */
  ifn = malloc (sizeof (IfStatementNode));
  ifn->left = ifn->right = NULL;
  ifn->op = RELOP_EQUAL;
  ifn->statement = NULL;

  /* return the IF statement node */
  return ifn;
}

/*
 * IF statement destructor
 * params:
 *   IfStatementNode*   ifn   the doomed IF statement
 */
void statement_destroy_if (IfStatementNode *ifn) {
  if (ifn->left)
    expression_destroy (ifn->left);
  if (ifn->right)
    expression_destroy (ifn->right);
  if (ifn->statement)
    statement_destroy (ifn->statement);
  free (ifn);
}

/*
 * IF statement output
 * params:
 *   IfStatementNode*   ifn   data for the IF statement
 * returns:
 *   char*                    the IF statement text
 */
char *statement_output_if (IfStatementNode *ifn) {

  /* local variables */
  char
    *if_text = NULL, /* the LET text to be assembled */
    *left_text = NULL, /* the text of the left expression */
    *op_text = NULL, /* the operator text */
    *right_text = NULL, /* the text of the right expression */
    *statement_text = NULL; /* the text of the conditional statement */

  /* assemble the expressions and conditional statement */
  left_text = expression_output (ifn->left);
  right_text = expression_output (ifn->right);
  statement_text = statement_output (ifn->statement);

  /* work out the operator text */
  op_text = malloc (3);
  switch (ifn->op) {
    case RELOP_EQUAL: strcpy (op_text, "="); break;
    case RELOP_UNEQUAL: strcpy (op_text, "<>"); break;
    case RELOP_LESSTHAN: strcpy (op_text, "<"); break;
    case RELOP_LESSOREQUAL: strcpy (op_text, "<="); break;
    case RELOP_GREATERTHAN: strcpy (op_text, ">"); break;
    case RELOP_GREATEROREQUAL: strcpy (op_text, ">="); break;
  }

  /* assemble the final IF text, if we have everything we need */
  if (left_text && op_text && right_text && statement_text) {
    if_text = malloc (3 + strlen (left_text) + strlen (op_text) +
      strlen (right_text) + 6 + strlen (statement_text) + 1);
    sprintf (if_text, "IF %s%s%s THEN %s", left_text, op_text, right_text,
      statement_text);
  }

  /* free up the temporary bits of memory we've reserved */
  if (left_text) free (left_text);
  if (op_text) free (op_text);
  if (right_text) free (right_text);
  if (statement_text) free (statement_text);

  /* return it */
  return if_text;
}


/*
 * GOTO Statement Functions
 */


/*
 * GOTO Statement Constructor
 * returns:
 *   GotoStatementNode*   the new GOTO statement
 */
GotoStatementNode *statement_create_goto (void) {

  /* local variables */
  GotoStatementNode *goton; /* the statement to create */

  /* create and initialise the data */
  goton = malloc (sizeof (GotoStatementNode));
  goton->label = NULL;

  /* return the goto statement */
  return goton;
}

/*
 * GOTO Statement Destructor
 * params:
 *   GotoStatementNode*   goton   the doomed GOTO statement
 */
void statement_destroy_goto (GotoStatementNode *goton) {
  if (goton) {
    if (goton->label) free (goton->label);
    free (goton);
  }
}

/*
 * GOTO statement output
 * params:
 *   GotoStatementNode*   goton   data for the GOTO statement
 * returns:
 *   char*                        the GOTO statement text
 */
char *statement_output_goto (GotoStatementNode *goton) {

  /* local variables */
  char
    *goto_text = NULL, /* the GOTO text to be assembled */
    *expression_text = NULL; /* the text of the expression */

  /* assemble the expression */
  expression_text = expression_output (goton->label);

  /* assemble the final LET text, if we have an expression */
  if (expression_text) {
    goto_text = malloc (6 + strlen (expression_text));
    sprintf (goto_text, "GOTO %s", expression_text);
    free (expression_text);
  }

  /* return it */
  return goto_text;
}


/*
 * GOSUB Statement Functions
 */


/*
 * GOSUB Statement Constructor
 * returns:
 *   GosubStatementNode*   the new GOSUB statement
 */
GosubStatementNode *statement_create_gosub (void) {

  /* local variables */
  GosubStatementNode *gosubn; /* the statement to create */

  /* create and initialise the data */
  gosubn = malloc (sizeof (GosubStatementNode));
  gosubn->label = NULL;

  /* return the gosub statement */
  return gosubn;
}

/*
 * GOSUB Statement Destructor
 * params:
 *   GosubStatementNode*   gosubn   the doomed GOSUB statement
 */
void statement_destroy_gosub (GosubStatementNode *gosubn) {
  if (gosubn) {
    if (gosubn->label) free (gosubn->label);
    free (gosubn);
  }
}

/*
 * GOSUB statement output
 * params:
 *   GosubStatementNode*   gosubn   data for the GOSUB statement
 * returns:
 *   char*                        the GOSUB statement text
 */
char *statement_output_gosub (GosubStatementNode *gosubn) {

  /* local variables */
  char
    *gosub_text = NULL, /* the GOSUB text to be assembled */
    *expression_text = NULL; /* the text of the expression */

  /* assemble the expression */
  expression_text = expression_output (gosubn->label);

  /* assemble the final LET text, if we have an expression */
  if (expression_text) {
    gosub_text = malloc (7 + strlen (expression_text));
    sprintf (gosub_text, "GOSUB %s", expression_text);
    free (expression_text);
  }

  /* return it */
  return gosub_text;
}


/*
 * END Statement Functions
 */


/*
 * END statement output
 * returns:
 *   char*   A new string with the text "END"
 */
char *statement_output_end (void) {
    char *end_text; /* the full text of the END command */
    end_text = malloc (4);
    strcpy (end_text, "END");
    return end_text;
}


/*
 * RETURN Statement Functions
 */


/*
 * RETURN statement output
 * returns:
 *   char*   A new string with the text "RETURN"
 */
char *statement_output_return (void) {
    char *return_text; /* the full text of the RETURN command */
    return_text = malloc (7);
    strcpy (return_text, "RETURN");
    return return_text;
}


/*
 * PRINT Statement Functions
 */


/*
 * PRINT statement constructor
 * returns:
 *   PrintStatementNode*   the created PRINT statement
 */
PrintStatementNode *statement_create_print (void) {

  /* local variables */
  PrintStatementNode *printn; /* the created node */

  /* allocate memory and assign safe defaults */
  printn = malloc (sizeof (PrintStatementNode));
  printn->first = NULL;

  /* return the PRINT statement node */
  return printn;
}

/*
 * Destructor for a PRINT statement
 * params:
 *   PrintStatementNode   *printn   the doomed PRINT statement.
 */
void statement_destroy_print (PrintStatementNode *printn) {
  OutputNode *current, *next;
  current = printn->first;
  while (current) {
    next = current->next;
    free (current);
    current = next;
  }
  free (printn);
}

/*
 * PRINT statement output
 * params:
 *   PrintStatementNode*   printn   data for the PRINT statement
 * returns:
 *   char*                          the PRINT statement text
 */
char *statement_output_print (PrintStatementNode *printn) {

  /* local variables */
  char
    *print_text, /* the PRINT text to be assembled */
    *output_text = NULL; /* the text of the current output item */
  OutputNode *output; /* the current output item */

  /* initialise the PRINT statement */
  print_text = malloc (6);
  strcpy (print_text, "PRINT");

  /* add the output items */
  if ((output = printn->first)) {
    do {

      /* add the separator */
      print_text = realloc (print_text, strlen (print_text) + 2);
      strcat (print_text, output == printn->first ? " " : ",");

      /* format the output item */
      switch (output->class) {
      case OUTPUT_STRING:
        output_text = malloc (strlen (output->output.string) + 3);
        sprintf (output_text, "%c%s%c", '"', output->output.string, '"');
        break;
      case OUTPUT_EXPRESSION:
        output_text = expression_output (output->output.expression);
        break;
      }

      /* add the output item */
      print_text = realloc (print_text,
        strlen (print_text) + strlen (output_text) + 1);
      strcat (print_text, output_text);
      free (output_text);

    /* look for the next output item */
    } while ((output = output->next));
  }

  /* return the assembled text */
  return print_text;
}


/*
 * INPUT Statement Functions
 */


/*
 * INPUT statement constructor
 * returns:
 *   InputStatementNode*   initialised INPUT statement data
 */
InputStatementNode *statement_create_input (void) {

  /* local variables */
  InputStatementNode *inputn; /* the new input statement data */

  /* allocate memory and initalise safely */
  inputn = malloc (sizeof (InputStatementNode));
  inputn->first = NULL;

  /* return the created node */
  return inputn;
}

/*
 * INPUT statement destructor
 * params:
 *   InputStatementNode*   inputn   the doomed INPUT statement node
 */
void statement_destroy_input (InputStatementNode *inputn) {

  /* local variables */
  VariableListNode
    *variable, /* the current variable to destroy */
    *next; /* the next variable to destroy */

  /* delete the variables from the variable list, then the input node */
  if (inputn) {
    variable = inputn->first;
    while (variable) {
      next = variable->next;
      free (variable);
      variable = next;
    }
    free (inputn);
  }
}

/*
 * INPUT statement output
 * params:
 *   InputStatementNode*   inputn   the input statement node to show
 * returns:
 *   char *                         the text of the INPUT statement
 */
char *statement_output_input (InputStatementNode *inputn) {

  /* local variables */
  char
    *input_text, /* the INPUT text to be assembled */
    var_text[3]; /* text representation of each variable with separator */
  VariableListNode *variable; /* the current output item */

  /* initialise the INPUT statement */
  input_text = malloc (6);
  strcpy (input_text, "INPUT");

  /* add the output items */
  if ((variable = inputn->first)) {
    do {
      sprintf (var_text, "%c%c",
        (variable == inputn->first) ? ' ' : ',',
        variable->variable + 'A' - 1);
      input_text = realloc (input_text, strlen (input_text) + 3);
      strcat (input_text, var_text);
    } while ((variable = variable->next));
  }

  /* return the assembled text */
  return input_text;
}


/*
 * Top Level Functions
 */


/*
 * Statement constructor
 * returns:
 *   StatementNode*   the newly-created blank statement
 */
StatementNode *statement_create (void) {

  /* local variables */
  StatementNode *statement; /* the created statement */

  /* allocate memory and set defaults */
  statement = malloc (sizeof (StatementNode));
  statement->class = STATEMENT_NONE;

  /* return the created statement */
  return statement;
}

/*
 * Statement destructor
 * params:
 *   StatementNode*   statement   the doomed statement
 */
void statement_destroy (StatementNode *statement) {
  switch (statement->class) {
    case STATEMENT_LET:
      statement_destroy_let (statement->statement.letn);
      break;
    case STATEMENT_PRINT:
      statement_destroy_print (statement->statement.printn);
      break;
    case STATEMENT_INPUT:
      statement_destroy_input (statement->statement.inputn);
      break;
    case STATEMENT_IF:
      statement_destroy_if (statement->statement.ifn);
      break;
    case STATEMENT_GOTO:
      statement_destroy_goto (statement->statement.goton);
      break;
    case STATEMENT_GOSUB:
      statement_destroy_gosub (statement->statement.gosubn);
      break;
    default:
      break;
  }
  free (statement);
}

/*
 * Statement output
 * params:
 *   StatementNode*   statement   the statement to output
 * returns:
 *   char*                        a string containing the statement line
 */
char *statement_output (StatementNode *statement) {

  /* local variables */
  char *output = NULL; /* the text output */

  /* build the statement itself */
  switch (statement->class) {
    case STATEMENT_LET:
      output = statement_output_let (statement->statement.letn);
      break;
    case STATEMENT_IF:
      output = statement_output_if (statement->statement.ifn);
      break;
    case STATEMENT_GOTO:
      output = statement_output_goto (statement->statement.goton);
      break;
    case STATEMENT_GOSUB:
      output = statement_output_gosub (statement->statement.gosubn);
      break;
    case STATEMENT_RETURN:
      output = statement_output_return ();
      break;
    case STATEMENT_END:
      output = statement_output_end ();
      break;
    case STATEMENT_PRINT:
      output = statement_output_print (statement->statement.printn);
      break;
    case STATEMENT_INPUT:
      output = statement_output_input (statement->statement.inputn);
      break;
    default:
      output = malloc (24);
      strcpy (output, "Unrecognised statement.");
  }

  /* return the listing line */
  return output;
}

/*
 * Program Line Constructor
 * returns:
 *   ProgramLineNode*   the new program line
 */
ProgramLineNode *program_line_create (void) {

  /* local variables */
  ProgramLineNode *program_line; /* the program line to create */

  /* create and initialise the program line */
  program_line = malloc (sizeof (ProgramLineNode));
  program_line->label = 0;
  program_line->statement = NULL;
  program_line->next = NULL;

  /* return the new program line */
  return program_line;
}

/*
 * Program Line Destructor
 * params:
 *   ProgramLineNode*   program_line   the doomed program line
 * params:
 *   ProgramLineNode*                  the next program line
 */
ProgramLineNode *program_line_destroy (ProgramLineNode *program_line) {

  /* local variables */
  ProgramLineNode *next = NULL; /* the next program line */

  /* record the next line and destroy this one */
  if (program_line) {
    next = program_line->next;
    if (program_line->statement)
      statement_destroy (program_line->statement);
    free (program_line);
  }

  /* return the line following */
  return next;
}

/*
 * Program Line Output
 * params:
 *   ProgramLineNode*   program_line   the line to output
 * returns:
 *   char*                             the reconstructed line
 */
char *program_line_output (ProgramLineNode *program_line) {

  /* local variables */
  char
    label_text [7], /* line label text */
    *output = NULL, /* the rest of the output */
    *line = NULL; /* the assembled line */

  /* initialise the line label */
  if (program_line->label)
    sprintf (label_text, "%5d ", program_line->label);
  else
    strcpy (label_text, "      ");

  /* build the statement itself */
  output = statement_output (program_line->statement);

  /* combine the two */
  line = malloc (strlen (label_text) + strlen (output) + 2);
  sprintf (line, "%s%s\n", label_text, output);
  free (output);

  /* return the listing line */
  return line;
}

/*
 * Program Constructor
 * returns:
 *   ProgramNode*   the constructed program
 */
ProgramNode *program_create (void) {

  /* local variables */
  ProgramNode *program; /* new program */

  /* create and initialise the program */
  program = malloc (sizeof (program));
  program->first = NULL;

  /* return the new program */
  return program;
}

/*
 * Program Destructor
 * params:
 *   ProgramNode*   program   the doomed program
 */
void program_destroy (ProgramNode *program) {

  /* local variables */
  ProgramLineNode *program_line; /* the program line to destroy */

  /* destroy the program lines, then the program itself */
  program_line = program->first;
  while (program_line)
    program_line = program_line_destroy (program_line);
  free (program);
}