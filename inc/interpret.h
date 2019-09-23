/*
 * Tiny BASIC Interpreter and Compiler Project
 * Interpreter header
 *
 * Released as Public Domain by Damian Gareth Walker 2019
 * Created: 23-Aug-2019
 */


#ifndef __INTERPRET_H__
#define __INTERPRET_H__


/*
 * Function Declarations
 */


/*
 * Interpret the program from the beginning
 * params:
 *   ProgramNode*   program   the program to interpret
 */
void interpret_program (ProgramNode *program);


#endif
