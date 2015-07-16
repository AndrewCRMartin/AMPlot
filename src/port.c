#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "port.h"

int ReqMessage(int wind, char *string, int flag)
{
   fprintf(stderr,"%s\n",string);

   return(TRUE);
}

int ReqMessage2(int wind, char *string1, char *string2, int flag)
{
   fprintf(stderr,"%s\n",string1);
   fprintf(stderr,"%s\n",string2);

   return(TRUE);
}

int ReqMessage3(int wind, char *string1, char *string2, char *string3, int flag)
{
   fprintf(stderr,"%s\n",string1);
   fprintf(stderr,"%s\n",string2);
   fprintf(stderr,"%s\n",string3);

   return(TRUE);
}

void WriteMessage(char *string)
{
   fprintf(stderr,"%s\n",string);
}

void WaitPointer(int win)
{
}

void ClearPointer(int win)
{
}

void Die(char *string)
{
   fprintf(stderr,"Fatal Error: %s\n", string);
   exit(1);
}

/************************************************************************/
/*>void StringToLower(char *string1, char *string2)
   ------------------------------------------------
   Input:      char  *string1       A character string
   Output:     char  *string2       Lower case version of string1
   
   This routine converts a lower or mixed case string to lower case.
   
   06.02.91 Original
   28.05.92 ANSIed
   07.01.93 Checks case before converting for SysV
*/
void StringToLower(char *string1,
                   char *string2)
{
   int i;
   
   for(i=0;i<strlen(string1);i++)
      if(isupper(string1[i])) 
         string2[i]=tolower(string1[i]);
      else
         string2[i]=string1[i];
   string2[i]='\0';
}


/************************************************************************/
/*>void StringToUpper(char *string1, char *string2)
   ------------------------------------------------
   Input:      char  *string1       A character string
   Output:     char  *string2       Upper case version of string1
   
   This routine converts a lower or mixed case string to upper case.
   
   06.02.91 Original
   28.05.92 ANSIed
   07.01.93 Checks case before converting for SysV
*/
void StringToUpper(char *string1,
                   char *string2)
{
   int i;
   
   for(i=0;i<strlen(string1);i++)
      if(islower(string1[i])) 
         string2[i]=toupper(string1[i]);
      else
         string2[i]=string1[i];
   string2[i]='\0';
}


/************************************************************************/
/*>char *KillLeadSpaces(char *string)
   ----------------------------------
   Input:      char  *string        A character string
   Returns:   *char                 A pointer to the string with the 
                                    leading spaces removed

   This routine strips leading spaces and tabs from a string returning
   a pointer to the first non-whitespace character.

   N.B. THE MACRO KILLLEADSPACES() MAY NOW BE USED INSTEAD
   
   06.02.91 Original
   28.05.92 ANSIed
   06.07.93 Added tab skipping
*/
char *KillLeadSpaces(char *string)
{
   while (*string == ' ' || *string == '\t') string++;
   return(string);
}


