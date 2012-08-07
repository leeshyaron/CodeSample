/*------------------------------------------------------------------*/
/* hist.h                                                           */
/* Author: Mark Xia                                                 */
/*------------------------------------------------------------------*/

#ifndef HIST_INCLUDED
#define HIST_INCLUDED

int histHasCommandPrefix(char *pcLine);
/* Return TRUE if the string pointed to by pcLine contains a !, 
   or FALSE otherwise. It is a checked runtime error for pcLine to
   be NULL. */

int histExpandLine(char *pcLine, DynArray_T oHistList, 
                   char *pcProgName);
/* If pcLine contains any !commandprefix, then properly expand
   !commandprefix by matching it with the most recently executed
   command that has commandprefix as a prefix. The resulting input
   line is updated in pcLine. Return TRUE if successful, and FALSE
   if !commandprefix could not be expanded. In the latter case, 
   print an error message to stderr. pcProgName is used in printing
   error messages. It is a checked runtime error for pcLine, oHistList,
   or pcProgName to be NULL. */

#endif                      /* HIST_INCLUDED */
