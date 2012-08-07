/*------------------------------------------------------------------*/
/* parse.h                                                          */
/* Author: Mark Xia                                                 */
/*------------------------------------------------------------------*/

#ifndef PARSE_INCLUDED
#define PARSE_INCLUDED

typedef struct Command *Command_T;
/* A Command_T is a sequence of tokens. A Command_T consists of a
   command name, a list of arguments, an indication of whether stdin
   should be redirected (and if so, to which stream), and an 
   indication of whether stdout should be redirected (and if so, to
   which stream). */

Command_T Command_new(void);
/* Create and return a Command whose oCommand, pcStdin, and pcStdout
   all point to NULL. The caller owns the Command. */

void Command_free(void *pvItem, void *pvExtra);
/* Free command pvItem. pvExtra is unused. It is a checked
   runtime error for pvItem to be NULL. */

char **Command_getArray(void *pvItem, void *pvExtra);
/* Return the ppcArray pointed to by command pvItem. pvExtra is 
   unused. It is a checked runtime error for pvItem to be NULL. */

int Command_getNumArg(void *pvItem, void *pvExtra);
/* Return the number of arguments in ppcArray pointed to by command
   pvItem. pvExtra is unused. It is a checked runtime error for 
   pvItem to be NULL. */

char *Command_getStdin(void *pvItem, void *pvExtra);
/* Return the string pcStdin pointed to by command pvItem. pvExtra 
   is unused. It is a checked runtime error for pvItem to be NULL. */

char *Command_getStdout(void *pvItem, void *pvExtra);
/* Return the string pcStdout pointed to by command pvItem. pvExtra
   is unused. It is a checked runtime error for pvItem to be NULL. */

int parseToken(const DynArray_T oTokens, Command_T command, 
               char *pcProgName);
/* Syntactically analyze oTokens. Make command point to a newly 
   created command that corresponds to oTokens. Return TRUE if 
   successful, and FALSE if oTokens contains a syntactical error.  
   In the latter case, print an error to stderr. pcProgName is used
   in printing error messages.
   It is a checked runtime error for oTokens, oCommand, and pcProgname
   to be NULL. */

#endif                      /* PARSE_INCLUDED */

