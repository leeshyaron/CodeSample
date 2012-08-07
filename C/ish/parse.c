/*------------------------------------------------------------------*/
/* parse.c                                                          */
/* Author: Mark Xia                                                 */
/* -----------------------------------------------------------------*/

#include "dynarray.h"
#include "parse.h"
#include "lexi.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*------------------------------------------------------------------*/

enum {FALSE, TRUE};

enum TokenType {TOKEN_WORD, TOKEN_STDIN, TOKEN_STDOUT};

/*------------------------------------------------------------------*/

/* A Command consists of a command name, a list of arguments, an 
   indication of whether stdin should be redirected (and if so, to
   which stream), and an indication of whether stdout should be 
   redirected (and if so, to which stream). */

struct Command
{
   /* The array consisting of strings. The strings 
      for Stdinredirect and Stdoutredirect are excluded. The first
      string specifies the command name. The last element points to
      NULL. */
   char **ppcArray;

   /* The number of arguments in ppcArray. This does not count the
      first element that points to the command name and the last
      element that points to NULL. */
   int iNumArg;
      
   /* The stream to which stdin should be redirected. */
   char *pcStdin;   

   /* The stream to which stdout should be redirected. */
   char *pcStdout;   

};

/*------------------------------------------------------------------*/

Command_T Command_new(void)

/* Create and return a Command whose oCommand, pcStdin, and pcStdout
   all point to NULL. */

{
   Command_T oCommand;

   oCommand = (struct Command*)malloc(sizeof(struct Command));
   assert(oCommand != NULL);

   oCommand->ppcArray = NULL;
   oCommand->iNumArg = 0;
   oCommand->pcStdin = NULL;
   oCommand->pcStdout = NULL;

   return oCommand;
}

/*------------------------------------------------------------------*/

void Command_free(void *pvItem, void *pvExtra)

/* Free command pvItem. pvExtra is unused. It is a checked
   runtime error for pvItem to be NULL. */

{
   struct Command *psCommand;

   assert(pvItem != NULL);

   psCommand = (struct Command*)pvItem;
   free(psCommand->ppcArray);
   free(psCommand->pcStdin);
   free(psCommand->pcStdout);
   free(psCommand);
}

/*------------------------------------------------------------------*/

char **Command_getArray(void *pvItem, void *pvExtra)

/* Return the ppcArray pointed to by command pvItem. pvExtra is 
   unused. It is a checked runtime error for pvItem to be NULL. */

{
   struct Command *psCommand;

   assert(pvItem != NULL);

   psCommand = (struct Command*)pvItem;
   return psCommand->ppcArray;
}

/*------------------------------------------------------------------*/

int Command_getNumArg(void *pvItem, void *pvExtra)

/* Return the number of arguments in ppcArray pointed to by command
   pvItem. pvExtra is unused. It is a checked runtime error for 
   pvItem to be NULL. */

{
   struct Command *psCommand;

   assert(pvItem != NULL);

   psCommand = (struct Command*)pvItem;
   return psCommand->iNumArg;
}

/*------------------------------------------------------------------*/

char *Command_getStdin(void *pvItem, void *pvExtra)

/* Return the string pcStdin pointed to by command pvItem. pvExtra 
   is unused. It is a checked runtime error for pvItem to be NULL. */

{
   struct Command *psCommand;
   
   assert(pvItem != NULL);

   psCommand = (struct Command*)pvItem;
   return psCommand->pcStdin;
}

/*------------------------------------------------------------------*/

char *Command_getStdout(void *pvItem, void *pvExtra)

/* Return the string pcStdout pointed to by command pvItem. pvExtra
   is unused. It is a checked runtime error for pvItem to be NULL. */

{
   struct Command *psCommand;

   assert(pvItem != NULL);

   psCommand = (struct Command*)pvItem;
   return psCommand->pcStdout;
}

/*------------------------------------------------------------------*/

static char **toCharArray(DynArray_T oTokens)

/* Return an array of char pointers consisting of the token value of
   each token in oTokens. It is a checked runtime error for oTokens
   to be NULL. */

{
   char *pcValue;
   char **ppcArray;
   DynArray_T oTemp;
   int i;
   int iLength;
   void *pvToken;
   
   assert(oTokens != NULL);

   oTemp = DynArray_new(0);
   for(i = 0; i < DynArray_getLength(oTokens); i++)
   {
      pvToken = DynArray_get(oTokens, i);
      pcValue = Token_getValue(pvToken, NULL);
      DynArray_add(oTemp, pcValue);
   }

   iLength = DynArray_getLength(oTemp);
   ppcArray = (char**)calloc((size_t)iLength + 1, sizeof(char*));
   assert(ppcArray != NULL);
   DynArray_toArray(oTemp, (void**)ppcArray);
   ppcArray[iLength] = NULL;
   DynArray_free(oTemp);
   return ppcArray;
}

/*------------------------------------------------------------------*/

int parseToken(const DynArray_T oTokens, Command_T oCommand, 
               char *pcProgName)

/* Syntactically analyze oTokens. Make command point to a newly 
   created command that corresponds to oTokens. Return TRUE if 
   successful, and FALSE if oTokens contains a syntactical error.  
   In the latter case, print an error to stderr. pcProgName is used
   in printing error messages.
   It is a checked runtime error for oTokens, oCommand, and pcProgName
   to be NULL. */

{
   int i = 0;
   char **ppcArray;
   struct Token *psToken;
   struct Token *psNextToken;

   assert(oTokens != NULL);
   assert(oCommand != NULL);
   assert(pcProgName != NULL);

   psToken = (struct Token*)DynArray_get(oTokens, i);
   if(Token_getType(psToken, NULL) != TOKEN_WORD)
   {
      fprintf(stderr, "%s: Missing command name\n", pcProgName);
      return FALSE;
   }

   for(i = 1; i < DynArray_getLength(oTokens); i++)
   {
      psToken = (struct Token*)(DynArray_get(oTokens, i));
      if(Token_getType(psToken, NULL) == TOKEN_STDIN)
      {
         (void)DynArray_removeAt(oTokens, i);
         Token_free(psToken, NULL);
         if(i == DynArray_getLength(oTokens))
         {
            fprintf(stderr, "%s: Standard input ", pcProgName);
            fprintf(stderr, "redirection without file name\n");
            return FALSE;
         }
         psNextToken = (struct Token*)DynArray_removeAt(oTokens, i);
         if(Token_getType(psNextToken, NULL) != TOKEN_WORD)
         {
            fprintf(stderr, "%s: Standard input ", pcProgName);
            fprintf(stderr, "redirection without file name\n");
            Token_free(psNextToken, NULL);
            return FALSE;
         }
         if(oCommand->pcStdin != NULL)
         {
            fprintf(stderr, "%s: Multiple redirection of ", pcProgName);
            fprintf(stderr, "standard input\n");
            Token_free(psNextToken, NULL);
            return FALSE;
         }
         oCommand->pcStdin = Token_getValue(psNextToken, NULL);
         free(psNextToken);
         i--;
      }
      else if(Token_getType(psToken, NULL) == TOKEN_STDOUT)
      {
         (void)DynArray_removeAt(oTokens, i);
         Token_free(psToken, NULL);
         if(i == DynArray_getLength(oTokens))
         {
            fprintf(stderr, "%s: Standard output ", pcProgName);
            fprintf(stderr, "redirection without file name\n");
            return FALSE;
         }
         psNextToken = (struct Token*)DynArray_removeAt(oTokens, i);
         if(Token_getType(psNextToken, NULL) != TOKEN_WORD)
         {
            fprintf(stderr, "%s: Standard output ", pcProgName);
            fprintf(stderr, "redirection without file name\n");
            Token_free(psNextToken, NULL);
            return FALSE;
         }
         if(oCommand->pcStdout != NULL)
         {
            fprintf(stderr, "%s: Multiple redirection of ", pcProgName);
            fprintf(stderr, "standard output\n");
            Token_free(psNextToken, NULL);
            return FALSE;
         }
         oCommand->pcStdout = Token_getValue(psNextToken, NULL);
         free(psNextToken);
         i--;
      }
   }

   ppcArray = toCharArray(oTokens);
   oCommand->ppcArray = ppcArray;
   oCommand->iNumArg = DynArray_getLength(oTokens) - 1;
   return TRUE;
}
