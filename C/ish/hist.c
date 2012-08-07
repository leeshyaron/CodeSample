/*------------------------------------------------------------------*/
/* hist.c                                                           */
/* Author: Mark Xia                                                 */
/* -----------------------------------------------------------------*/

#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*------------------------------------------------------------------*/

enum {FALSE, TRUE};
enum {MAX_LINE_SIZE = 1024};

/*------------------------------------------------------------------*/

int histHasCommandPrefix(char *pcLine)

/* Return TRUE if the string pointed to by pcLine contains a !, 
   or FALSE otherwise. It is a checked runtime error for pcLine to
   be NULL. */

{
   int i;
   
   assert(pcLine != NULL);

   for(i = 0; pcLine[i] != '\0'; i++)
      if(pcLine[i] == '!')
         return TRUE;

   return FALSE;
}

/*------------------------------------------------------------------*/

static int isTerminatorChar(char c)

/* Return TRUE if c is a terminator character, and FALSE otherwise. */
{
   return (c == ' ' || c == '\t' || c == '\n' || 
           c == '>' || c == '<'  || c == '\0');
}

/*------------------------------------------------------------------*/

static char *findPrevCommand(char *pcLine, DynArray_T oHistList)

/* Return the most recently executed command that matches the
   !commandprefix pcLine points to. Return NULL if a match cannot be 
   found.  
   It is a checked runtime error for pcLine or oHistList to be NULL.
   It is a checked runtime error for pcLine to not point to ! of 
   !commandprefix. It is a checked runtime error for pcLine to 
   point to an empty commandprefix. */

{
   int iLength;
   int i;
   char *pc;

   assert(pcLine != NULL);
   assert(pcLine[0] == '!');
   assert(!isTerminatorChar(pcLine[1]));
   assert(oHistList != NULL);

   if(DynArray_getLength(oHistList) == 0)
      return NULL;
          
   /* Note pcLine[1] is not a terminator character. */ 
   iLength = 1;

   while(!isTerminatorChar(pcLine[iLength + 1]))
      iLength++;

   /* Find a matching command starting from the end of oHistList 
      because the end represents the most recent command. */
   for(i = DynArray_getLength(oHistList) - 1; i >= 0; i--)
   {
      pc = (char*)DynArray_get(oHistList, i);
      if(strncmp(&pcLine[1], pc, (size_t)iLength) == 0)
         return pc;
   }

   return NULL;
}

/*------------------------------------------------------------------*/

int histExpandLine(char *pcLine, DynArray_T oHistList, char *pcProgName)

/* If pcLine contains any !commandprefix, then properly expand
   !commandprefix by matching it with the most recently executed
   command that has commandprefix as a prefix. The resulting input
   line is updated in pcLine. Return TRUE if successful, and FALSE
   if !commandprefix could not be expanded. In the latter case, 
   print an error message to stderr. pcProgName is used in printing
   error messages. It is a checked runtime error for pcLine, oHistList,
   or pcProgName to be NULL. */

{
   char acLine[MAX_LINE_SIZE];
   char *pcPrevCommand;
   int i;
   int j = 0;
   int iLength;
   
   assert(pcLine != NULL);
   assert(oHistList != NULL);
   assert(pcProgName != NULL);

   for(i = 0; pcLine[i] != '\0'; i++)
   {
      if(pcLine[i] == '!')
      {
         /* Ignore the ! if commandprefix is empty. */
         if(!isTerminatorChar(pcLine[i + 1]))
         {
            pcPrevCommand = findPrevCommand(&pcLine[i], oHistList);
            if(pcPrevCommand == NULL)
            {
               /* Make pcLine + i point to the !commandprefix that 
                  did not have a match. */
               iLength = i;
               while(!isTerminatorChar(pcLine[iLength + 1]))
                  iLength++;
               pcLine[iLength + 1] = '\0';

               fprintf(stderr, "%s: %s: Event not found\n", pcProgName,
                       pcLine + i);
               return FALSE;
            }
            else
            {
               strcpy(&acLine[j], pcPrevCommand);
               j += strlen(pcPrevCommand);
            
               /* Jump to the the terminator character
                  of the current !commandprefix. */
               while(!isTerminatorChar(pcLine[i + 1]))
                  i++;
            }
         }
      }
      else
      {
         acLine[j] = pcLine[i];
         j++;
      }
   }
   acLine[j] = '\0';

   strcpy(pcLine, acLine);
   return TRUE;
}
