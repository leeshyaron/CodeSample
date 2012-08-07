/*------------------------------------------------------------------*/
/* exec.c                                                           */
/* Author: Mark Xia                                                 */
/* -----------------------------------------------------------------*/

#define _GNU_SOURCE
#include "dynarray.h"
#include "parse.h"
#include "lexi.h"
#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>

/*------------------------------------------------------------------*/

enum {PERMISSIONS = 0600};

/*------------------------------------------------------------------*/

static void callSetenv(char **ppcArray, int iNumArg, char *pcProgName)

/* Call setenv() iff the number of arguments iNumArg in ppcArray is 1 
   or 2. Print error message to stderr otherwise. pcProgName is used
   in printing error messages. It is a checked runtime error for the
   first element in ppcArray to not be "setenv". It is a checked 
   runtime error for ppcArray or pcProgName to be NULL. It is a 
   checked runtime error for iNumArg to be negative. */

{  
   assert(ppcArray != NULL);
   assert(strcmp(ppcArray[0], "setenv") == 0);
   assert(pcProgName != NULL);
   assert(iNumArg >= 0);

   if(iNumArg == 0)
      fprintf(stderr, "%s: setenv: Missing variable\n", pcProgName);
   else if (iNumArg > 2) 
      fprintf(stderr, "%s: setenv: Too many arguments\n", pcProgName);
   else if(iNumArg == 1)
   {
      if(setenv(ppcArray[1], "\0", 1) == -1)
         perror(pcProgName);
   }
   else if(iNumArg == 2)
   {
      if(setenv(ppcArray[1], ppcArray[2], 1) == -1)
         perror(pcProgName);
   }
}
   
/*------------------------------------------------------------------*/

static void callUnsetenv(char **ppcArray, int iNumArg, char *pcProgName)

/* Call unsetenv() iff the number of arguments iNumArg in ppcArray is
   1. Print error message to stderr otherwise. pcProgName is used in
   printing error messages. It is a checked runtime error for the
   first element in ppcArray to not be "unsetenv". It is a checked 
   runtime error for ppcArray or pcProgName to be NULL. It is a 
   checked runtime error for iNumArg to be negative. */

{
   assert(ppcArray != NULL);
   assert(strcmp(ppcArray[0], "unsetenv") == 0);
   assert(pcProgName != NULL);
   assert(iNumArg >= 0);
   
   if(iNumArg == 0)
      fprintf(stderr, "%s: unsetenv: Missing variable\n", pcProgName);
   else if(iNumArg > 1)
      fprintf(stderr, "%s: unsetenv: Too many arguments\n", pcProgName);
   else if(unsetenv(ppcArray[1]) == -1)
      perror(pcProgName);
}
   
/*------------------------------------------------------------------*/

static void callCd(char **ppcArray, int iNumArg, char *pcProgName)

/* Call chdir() iff the number of arguments iNumArg in ppcArray is 
   0 or 1.  Print error message to stderr otherwise. pcProgName is
   used in printing error messages. It is a checked runtime error 
   for the first element in ppcArray to not be "cd". It is a checked 
   runtime error for ppcArray or pcProgName to be NULL. It is a 
   checked runtime error for iNumArg to be negative. */

{
   char *pcEnv;

   assert(ppcArray != NULL);
   assert(strcmp(ppcArray[0], "cd") == 0);
   assert(pcProgName != NULL);
   assert(iNumArg >= 0);

   pcEnv = getenv("HOME");

   if(iNumArg > 1)
      fprintf(stderr, "%s: cd: Too many arguments\n", pcProgName);
   else if(iNumArg == 0 && pcEnv == NULL)
      fprintf(stderr, "%s: cd: HOME not set\n", pcProgName);
   else if(iNumArg == 0 && pcEnv != NULL)
   {
      if(chdir(pcEnv) == -1)
      {
         fprintf(stderr, "%s: ", pcProgName);
         perror(pcEnv);
      }
   }  
   else if(chdir(ppcArray[1]) == -1)
   {
      fprintf(stderr, "%s: ", pcProgName);
      perror(ppcArray[1]);
   }  
}
   
/*------------------------------------------------------------------*/

static void callExit(char **ppcArray, int iNumArg, char *pcProgName)

/* Call exit() iff the number of arguments iNumArg in ppcArray is 0.
   Print error message to stderr otherwise. pcProgName is used in
   printing error messages.  It is a checked runtime error for the
   first element in ppcArray to not be "exit". It is a checked runtime
   error for ppcArray or pcProgName to be NULL. It is a checked runtime
   error for iNumArg to be negative. */

{
   assert(ppcArray != NULL);
   assert(strcmp(ppcArray[0], "exit") == 0);
   assert(pcProgName != NULL);
   assert(iNumArg >= 0);

   if(iNumArg > 0)
      fprintf(stderr, "%s: exit: Too many arguments\n", pcProgName);
   else
   {
      printf("\n");
      exit(EXIT_SUCCESS);
   }
}

/*------------------------------------------------------------------*/

static void callHistory(char **ppcArray, DynArray_T oHistList, 
                        int iNumArg, char *pcProgName)

/* Print out the history list iff the number of arguments iNumArg in 
   ppcArray is 0.  Print error message to stderr otherwise. pcProgName
   is used in printing error messages. It is a checked runtime error
   for ppcArray, oHistList, or pcProgName to be NULL. It is a checked 
   runtime error for the first element in ppcArray to not be "history".
   It is a checked runtime error for oHistList to be empty. It is a 
   checked runtime error for iNumArg to be negative. */

{
   int i;

   assert(ppcArray != NULL);
   assert(strcmp(ppcArray[0], "history") == 0);
   assert(oHistList != NULL);
   assert(DynArray_getLength(oHistList) != 0);
   assert(iNumArg >= 0);
   assert(pcProgName != NULL);

   if(iNumArg > 0)
      fprintf(stderr, "%s: history: Too many arguments\n", pcProgName);
   else
      for(i = 0; i < DynArray_getLength(oHistList); i++)
         printf("%d\t%s\n", i, (char*)DynArray_get(oHistList, i));
      
}
      
/*------------------------------------------------------------------*/

void execute(Command_T oCommand, DynArray_T oHistList, char *pcProgName)

/* Execute the command given by oCommand while properly handling any
   necessary input/output redirection. pcProgName is used in printing
   error messages. It is a checked runtime error for oCommand,
   oHistList, or pcProgName to be NULL. */
{
   char *pcStdin;
   char *pcStdout;
   char **ppcArray;
   int iNumArg;
   pid_t iPid;
   void (*pfRet)(int);

   assert(oCommand != NULL);
   assert(oHistList != NULL);
   assert(pcProgName != NULL);

   ppcArray = Command_getArray(oCommand, NULL);
   iNumArg = Command_getNumArg(oCommand, NULL);
   pcStdin = Command_getStdin(oCommand, NULL);
   pcStdout = Command_getStdout(oCommand, NULL);

   if(strcmp(ppcArray[0], "setenv") == 0)
      callSetenv(ppcArray, iNumArg, pcProgName);
   else if(strcmp(ppcArray[0], "unsetenv") == 0)
      callUnsetenv(ppcArray, iNumArg, pcProgName);
   else if(strcmp(ppcArray[0], "cd") == 0)
      callCd(ppcArray, iNumArg, pcProgName);
   else if(strcmp(ppcArray[0], "exit") == 0)
      callExit(ppcArray, iNumArg, pcProgName);   
   else
   {      
      fflush(NULL);
      iPid = fork();
      if (iPid == -1) {perror(pcProgName); exit(EXIT_FAILURE); }
 
      if (iPid == 0)
      {
         int iFd;
         int iRet;
         
         pfRet = signal(SIGINT, SIG_DFL);
         if(pfRet == SIG_ERR) {perror(pcProgName); exit(EXIT_FAILURE); }

         if(pcStdin != NULL)
         {
            iFd = open(pcStdin, O_RDONLY);
            if (iFd == -1) 
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdin); 
               exit(EXIT_FAILURE); 
            }
            iRet = close(0);
            if (iRet == -1)
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdin); 
               exit(EXIT_FAILURE); 
            }            
            iRet = dup(iFd);
            if (iRet == -1)
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdin); 
               exit(EXIT_FAILURE); 
            }
            iRet = close(iFd);
            if (iRet == -1)
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdin); 
               exit(EXIT_FAILURE); 
            }
         }
         if(pcStdout != NULL)
         {
            iFd = creat(pcStdout, PERMISSIONS);
            if (iFd == -1) 
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdout); 
               exit(EXIT_FAILURE); 
            }
            iRet = close(1);
            if (iRet == -1)
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdout); 
               exit(EXIT_FAILURE); 
            }
            iRet = dup(iFd);
            if (iRet == -1)
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdout); 
               exit(EXIT_FAILURE); 
            }
            iRet = close(iFd);
            if (iRet == -1) 
            {
               fprintf(stderr, "%s: ", pcProgName);
               perror(pcStdout); 
               exit(EXIT_FAILURE); 
            }
         }
         
         if(strcmp(ppcArray[0], "history") == 0)
         {
            callHistory(ppcArray, oHistList, iNumArg, pcProgName);   
            exit(EXIT_SUCCESS);
         }
         else
         {
            execvp(ppcArray[0], ppcArray);
            fprintf(stderr, "%s: ", pcProgName);
            perror(ppcArray[0]);
            exit(EXIT_FAILURE);
         }
      }
   
      iPid = wait(NULL);
      if (iPid == -1) {perror(pcProgName); exit(EXIT_FAILURE); }
   }
}
