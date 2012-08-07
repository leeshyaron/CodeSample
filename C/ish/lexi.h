/*------------------------------------------------------------------*/
/* lexi.h                                                           */
/* Author: Mark Xia                                                 */
/*------------------------------------------------------------------*/

#ifndef LEXI_INCLUDED
#define LEXI_INCLUDED

typedef struct Token *Token_T;
/* A Token_T is a word, a '<', or a '>', expressed as a string. */

void Token_free(void *pvItem, void *pvExtra);
/* Free token pvItem. pvExtra is unused. It is a checked runtime
   error for pvItem to be NULL. */

int Token_getType(void *pvItem, void *pvExtra);
/* Return the eType of token pvItem. It is a checked runtime
   error for pvItem to be NULL. */

char *Token_getValue(void *pvItem, void *pvExtra);
/* Return the pcValue of token pvItem. It is a checked runtime
   error for pvItem to be NULL. */

int lexLine(const char *pcLine, DynArray_T oTokens, char *pcProgName);
/* Lexically analyze string pcLine.  Populate oTokens with the 
   tokens that pcLine contains.  Return TRUE if successful, and FALSE
   if pcLine contains a lexical error.  In the latter case, oTokens
   may contain tokens that were discovered before the lexical error.
   pcProgName is used in printing error messages. It is a checked
   runtime error for pcLine, oTokens, or pcProgName to be NULL. It 
   is a checked runtime error for the size of pcLine to be greater
   than MAX_LINE_SIZE. */

/* lexLine() uses a DFA approach. It "reads" its characters from
   pcLine. */

#endif                      /* LEXI_INCLUDED */
