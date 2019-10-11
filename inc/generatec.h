/*
 * Tiny BASIC Interpreter and Compiler Project
 * C Output Header
 *
 * Copyright (C) Damian Gareth Walker 2019
 * Created: 03-Oct-2019
 */


#ifndef __GENERATEC_H__
#define __GENERATEC_H__


/* forward references */
typedef struct c_program CProgram;

/* object structure */
typedef struct c_program {
  void *private_data; /* private data */
  char *c_output; /* the generated C code */
  void (*generate) (CProgram *, ProgramNode *); /* generate function */
  void (*destroy) (CProgram *); /* destructor */
} CProgram;


/*
 * Function Declarations
 */


/*
 * Constructor
 * returns:
 *   CProgram*   the C program object
 */
CProgram *new_CProgram (void);


#endif