/*------------------------------------------------------------------*/
/* lexi.c                                                           */
/* Author: Mark Xia                                                 */
/* -----------------------------------------------------------------*/

#include "dynarray.h"
#include "lexi.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};

enum TokenType {TOKEN_WORD, TOKEN_STDIN, TOKEN_STDOUT};

enum LexState {STATE_START, STATE_IN_WORD, STATE_IN_QUOTE, 
               STATE_ERROR, STATE_EXIT};

/*------------------------------------------------------------------*/

/* A Token is a word, a '<', or a '>', expressed as a string. */

struct Token
{
   /* The type of the token. */  
   enum TokenType eType;
      
   /* The string which is the token's value. */
   char *pcValue;
};

/*------------------------------------------------------------------*/

void Token_free(void *pvItem, void *pvExtra)

/* Free token pvItem. pvExtra is unused. It is a checked runtime
   error for pvItem to be NULL. */

{
   struct Token *psToken;

   assert(pvItem != NULL);

   psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*------------------------------------------------------------------*/

int Token_getType(void *pvItem, void *pvExtra)

/* Return the eType of token pvItem. It is a checked runtime
   error for pvItem to be NULL. */

{
   struct Token *psToken;

   assert(pvItem != NULL);

   psToken = (struct Token*)pvItem;
   return psToken->eType;
}

/*------------------------------------------------------------------*/

char *Token_getValue(void *pvItem, void *pvExtra)

/* Return the pcValue of token pvItem. It is a checked runtime
   error for pvItem to be NULL. */

{
   struct Token *psToken;
   
   assert(pvItem != NULL);

   psToken = (struct Token*)pvItem;
   return psToken->pcValue;
}

/*------------------------------------------------------------------*/
     
static struct Token *makeToken(enum TokenType eTokenType,
                               char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue. It is a checked runtime error
   for eTokenType to not equal a TokenType. It is a checked runtime
   error for pcValue to be NULL. */

{
   struct Token *psToken;

   assert(eTokenType == TOKEN_WORD || eTokenType == TOKEN_STDOUT ||
          eTokenType == TOKEN_STDIN);
   assert(pcValue != NULL);

   psToken = (struct Token*)malloc(sizeof(struct Token));
   assert(psToken != NULL);

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   assert(psToken->pcValue != NULL);
   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*------------------------------------------------------------------*/

int lexLine(const char *pcLine, DynArray_T oTokens, char *pcProgName)

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

{
   char acValue[MAX_LINE_SIZE];
   char c;
   int iLineIndex = 0;
   int iValueIndex = 0;
   enum LexState eState = STATE_START;
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(strlen(pcLine) + 1 <= MAX_LINE_SIZE);
   assert(oTokens != NULL);
   assert(pcProgName != NULL);

   while ((eState != STATE_EXIT) && (eState != STATE_ERROR))
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];

      switch (eState)
      {
         case STATE_START:
            if ((c == '\n') || (c == '\0'))
               eState = STATE_EXIT;
            else if (c == '<')
            {
               /* Create a STDIN token. */
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_STDIN, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;

               eState = STATE_START;
            }
            else if (c == '>')
            {
               /* Create a STDOUT token. */
               acValue[iValueIndex++] = c;               
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_STDOUT, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;

               eState = STATE_START;
            }
            else if (c == '"')
            {
               eState = STATE_IN_QUOTE;
            }
            else if ((int)c > 0x20 && (int)c < 0x7F)
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
            else if ((c == ' ') || (c == '\t'))
               eState = STATE_START;
            else
               eState = STATE_ERROR;
            break;

         case STATE_IN_WORD:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               eState = STATE_EXIT;
            }
            else if ((c == ' ') || (c == '\t'))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               eState = STATE_START;
            }
            else if (c == '<')
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               /* Create a STDIN token. */
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_STDIN, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;

               eState = STATE_START;
            }
            else if (c == '>')
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               /* Create a STDOUT token. */
               acValue[iValueIndex++] = c;               
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_STDOUT, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               eState = STATE_START;
            }
            else if (c == '"')
            {
               eState = STATE_IN_QUOTE;
            }
            else if ((int)c > 0x20 && (int)c < 0x7F)
            {
               acValue[iValueIndex++] = c;               
               eState = STATE_IN_WORD;
            }
            else
               eState = STATE_ERROR;
            break;

         case STATE_IN_QUOTE:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a token so we know command did not consist of
                  entirely white spaces. The eType TOKEN_WORD is 
                  chosen arbitrarily. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               DynArray_add(oTokens, psToken);
               iValueIndex = 0;
               
               fprintf(stderr, "%s: Unmatched quote\n", pcProgName);
               eState = STATE_ERROR;
            }
            else if (c == '"')
            {
               eState = STATE_IN_WORD;
            }
            else if ((c == ' ') || (c == '\t') 
                                || ((int)c > 0x20 && (int)c < 0x7F))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_QUOTE;
            }
            else 
               eState = STATE_ERROR;
            break;

         default:
            assert(0);
      }
   }

   if (eState == STATE_ERROR)
      return FALSE;
   return TRUE;
}
