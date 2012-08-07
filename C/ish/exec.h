/*------------------------------------------------------------------*/
/* exec.h                                                           */
/* Author: Mark Xia                                                 */
/*------------------------------------------------------------------*/

#ifndef EXEC_INCLUDED
#define EXEC_INCLUDED

void execute(Command_T oCommand, DynArray_T oHistList, 
             char *pcProgName);
/* Execute the command given by oCommand while properly handling any
   necessary input/output redirection. pcProgName is used in printing
   error messages. It is a checked runtime error for oCommand,
   oHistList, or pcProgName to be NULL. */

#endif                      /* EXEC_INCLUDED */
