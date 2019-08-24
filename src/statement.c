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
 * Level 1 Functions
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
  case STATEMENT_PRINT:
    output = statement_output_print (statement->statement.printn);
    break;
  case STATEMENT_IF:
  case STATEMENT_GOTO:
  case STATEMENT_GOSUB:
  case STATEMENT_RETURN:
  case STATEMENT_END:
  case STATEMENT_INPUT:
    output = malloc (17);
    sprintf (output, "Statement type %d", statement->class);
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