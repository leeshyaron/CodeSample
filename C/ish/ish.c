/*------------------------------------------------------------------*/
/* ish.c                                                            */
/* Author: Mark Xia                                                 */
/* -----------------------------------------------------------------*/

#include "dynarray.h"
#include "lexi.h"
#include "parse.h"
#include "exec.h"
#include "hist.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

/*------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

/*------------------------------------------------------------------*/

static char *getIshrc(void)

/* Return a string pointing to the file location of the .ishrc file
   residing in the HOME directory. */

{
   char *pcHome;
   char *pcTemp;

   pcHome = getenv("HOME");
   pcTemp = malloc(strlen(pcHome) + strlen("/.ishrc") + 1);
   assert(pcTemp != NULL);
   strcpy(pcTemp, pcHome);
   strcat(pcTemp, "/.ishrc");
   return pcTemp;
}

/*------------------------------------------------------------------*/

static void performCommand(char *acLine, DynArray_T oHistoryList, 
                           char *pcProgName)

/* Expand any !commandprefix in acLine. Insert acLine into 
   oHistoryList iff the expanding succeeds and acLine does not consist
   of entirely whitespace characters. Lexically and syntactically 
   analyze acLine. Execute acLine if no errors are found. It is a 
   checked runtime error for acLine, oHistory, or pcProgName to be
   NULL. */

{
   char *pcTemp;
   Command_T oCommand;
   DynArray_T oTokens;
   int iSuccessful;

   assert(acLine != NULL);
   assert(oHistoryList != NULL);
   assert(pcProgName != NULL);

   if(histHasCommandPrefix(acLine))
   {
      iSuccessful = histExpandLine(acLine, oHistoryList, pcProgName);
      if(iSuccessful)
         printf("%s\n", acLine);
      else
         return;
   }
   oTokens = DynArray_new(0);
   iSuccessful = lexLine(acLine, oTokens, pcProgName);
   if(DynArray_getLength(oTokens) > 0)
   {
      /* Allocate memory to store command in oHistoryList iff 
         command does not consist of entirely whitespace
         characters. */
      pcTemp = (char*)malloc(strlen(acLine) + 1);
      assert(pcTemp != NULL);
      strcpy(pcTemp, acLine);
      DynArray_add(oHistoryList, pcTemp);

      if(iSuccessful)
      {
         oCommand = Command_new();
         iSuccessful = parseToken(oTokens, oCommand, pcProgName);
         if(iSuccessful)
            execute(oCommand, oHistoryList, pcProgName);
         Command_free(oCommand, NULL);
      }
   }
   DynArray_map(oTokens, Token_free, NULL);
   DynArray_free(oTokens);
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])

/* Read lines from the .ishrc file residing in the HOME directory
   until EOF is reached. Write each line that is read to
   stdout and execute each line. Read a line from stdin and
   execute it. Repeat until EOF. Return 0. */

{
   char acLine[MAX_LINE_SIZE];
   char *pcTemp;
   DynArray_T oHistoryList; 
   FILE *psFile;
   int i;
   void (*pfRet)(int);

   pfRet = signal(SIGINT, SIG_IGN);
   if(pfRet == SIG_ERR) {perror(argv[0]); exit(EXIT_FAILURE); }

   oHistoryList = DynArray_new(0);

   pcTemp = getIshrc();
   psFile = fopen(pcTemp, "r");
   free(pcTemp);

   while (psFile != NULL && 
          fgets(acLine, MAX_LINE_SIZE, psFile) != NULL)
   {
      /* Remove '\n' if acLine ends with '\n'. This is done so 
         the commands in the history list do not end with '\n',
         which is necessary to properly expand !commandprefix. */
      if(acLine[strlen(acLine)-1] == '\n')
         acLine[strlen(acLine)-1] = '\0';
     
      printf("%% %s\n", acLine);
      /* Explicitly flush the stdout buffer so we can test ish
         properly by redirecting the output to a file. */
      fflush(stdout);
      
      performCommand(acLine, oHistoryList, argv[0]);
   }
   printf("%% ");
   fflush(stdout);
   while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
   {
      /* Remove '\n' if acLine ends with '\n'. This is done so 
         the commands in the history list do not end with '\n',
         which is necessary to properly expand !commandprefix. */
      if(acLine[strlen(acLine)-1] == '\n')
         acLine[strlen(acLine)-1] = '\0';
      
      performCommand(acLine, oHistoryList, argv[0]);

      printf("%% ");
      fflush(stdout);
   }
   printf("\n");
   fflush(stdout);

   for(i = 0; i < DynArray_getLength(oHistoryList); i++)
      free(DynArray_get(oHistoryList, i));
   DynArray_free(oHistoryList);
   return 0;
}
