/*
 * Tiny BASIC
 * Listing Output Header
 *
 * Released as Public Domain by Damian Gareth Walker, 2019
 * Created: 18-Sep-2019
 */


/*
 * Function Declarations
 */


/*
 * Program Line Output
 * params:
 *   ProgramLineNode*   program_line     the line to output
 *   ErrorHandler*      listing_errors   error handler for listing
 * returns:
 *   char*                               the reconstructed line
 */
char *listing_line_output (ProgramLineNode *program_line,
  ErrorHandler *listing_errors);
