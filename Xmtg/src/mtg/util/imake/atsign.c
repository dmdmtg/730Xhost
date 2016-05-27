/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	NOIDENT
#ident	"@(#)imake:atsign.c	1.2"
#endif

#include <stdio.h>
#include <malloc.h>
#define QUESTIONABLE  -1

char * Reserved[] = 
   {
   "$(CFLAGS)",
   "$(YFLAGS)", 
   "$(YACC)",
   "$(LDFLAGS)", 
   "$(LD)",
   "$(LFLAGS)", 
   "$(LEX)",
   "$(GFLAGS)", 
   "$(GET)",
   "$(F77FLAGS)", 
   "$(F77)",
   "$(CFLAGS)",
   "$(CC)",
   "$(ASFLAGS)", 
   "$(AS)",
   "$(ARFLAGS)",
   "$(AR)",
   "$(MAKE)",
   "$(MAKEFLAGS)",
   NULL
   };

struct Lines
   {
   char * Literal;
   int Length;
   int UseFlag;
   int TabFlag;
   int StartChar;
   char * Variable;
   struct Lines * nextvar;
   struct Lines * next;
   };
struct Lines * Start;

struct Lines * StartVar;

char buffer[1000];
int DebugFlag = 0;

void ReadFile();
void RemoveBlanks();
void InsertTabs();
void DeleteUnreferencedVariables();
void WriteFile();
main(argc,argv)
int argc;
char * argv[];
{

ReadFile();
RemoveBlanks();
InsertTabs();
DeleteUnreferencedVariables();
WriteFile();

} /* end of main */
void ReadFile()
{
char c;
int i = 0;
struct Lines * Current = NULL;
struct Lines * StoreLine();

while ((c = getchar()) != (char)EOF)
   {
   switch (c)
      {
      case '\n': Current = StoreLine(Current,i);
                 i = 0;
                 break;
      case '@' : c = getchar();
                 switch (c)
                    {
                    case '\n': buffer[i++] = '@';
                               Current = StoreLine(Current,i);
                               i = 0;
                               break;
                    case '@' : Current = StoreLine(Current,i);
                               i = 0;
                               break;
                    default  : buffer[i++] = '@';
                               buffer[i++] = c;
                               break;
                    }
                 break;
      default  : buffer[i++] = c;
                 break;
      }

   }

}/* end of ReadFile */
struct Lines * StoreLine(current,length)
struct Lines * current;
int length;
{
struct Lines * temp;

buffer[length] = 0;

temp = (struct Lines *) malloc(sizeof (struct Lines));
temp-> Literal = malloc(length+1);
strcpy(temp-> Literal,buffer);
temp-> Length = length;
temp-> UseFlag = 1;
temp-> TabFlag = 0;
temp-> StartChar = 0;
temp-> Variable = NULL;
temp-> next = NULL;
temp-> nextvar = NULL;

if (current == NULL)
   Start = temp;
else
   current-> next = temp;

return temp;

} /* end of StoreLine */
void RemoveBlanks()
{
register i = 0;
struct Lines * p = NULL;
struct Lines * q = NULL;

for (q = p = Start; p != NULL; p = p-> next)
   {
   for (i = p-> Length - 1;
        (i >= 0) && (p-> Literal[i] == ' ' || p-> Literal[i] == '\t');
         i--)                                    ;
   p-> Literal[i + 1] = 0;
   p-> Length = i + 1;
   if (p-> Length == 0 && q-> Length == 0) p-> UseFlag = 0;
   q = p;
   }

} /* end of RemoveBlanks */
void InsertTabs()
{
struct Lines * p = NULL;
struct Lines * q = NULL;
int tabflag = 0;
register i = 0;

for (q = p = Start; p != NULL; p = p-> next)
   {
   if (tabflag)
      {
      if (p-> Length == 0)
          tabflag = 0;
      else
         if (p-> Literal[0] != '\t')
            p-> TabFlag = 1;
      }
   else
      {
      if (strchr(p-> Literal,':') && (p-> Literal[0] != '#'))
         tabflag = 1;
      else
         ExtractVariable(p);
      }
   if (tabflag)
      {
      for (i = 0; i < p-> Length && p->Literal[i] == ' '; i++) ;
      p-> StartChar = i;
      }
   }

} /* end of InsertTabs */
ExtractVariable(p)
struct Lines * p;
{
int i = 0;
struct Lines * v = p;
struct Lines * e = NULL;
struct Lines * ExtractWord();
char * V = NULL;
int len = 0;

/* find out if line(s) are formatted as 'bbbbbVARIABLEb=....' */

if ((v = ExtractWord(v,&i,&len," \t")) != NULL)
   {
   V = malloc(len + 4);
   strcpy(V,"$(");
   strcat(V,buffer);
   strcat(V,")");

   if ((e = ExtractWord(v,&i,&len," \t\\\000")) != NULL)
      {
      if (buffer[0] == '=' && !InSet(V,Reserved))
         {
         p-> nextvar = StartVar;
         StartVar = p;

         MarkLogical(p,QUESTIONABLE);
         p-> Variable = V;
         }
      else
         free(V);
      }
   }

} /* end of ExtractVariable */
InSet(p,s)
char * p;
char * s[];
{
register i = 0;

for (i = 0; s[i]; i++)
   if (!strcmp(p,s[i])) return 1;

return 0;
} /* end of InSet */
MarkLogical(p,u)
struct Lines * p;
int u;
{

while (p != NULL)
   {
   p-> UseFlag = u;
   if (p->Literal[strlen(p-> Literal) - 1] != '\\')
      break;
   p = p-> next;
   }

} /* end of MarkLogical */
struct Lines * ExtractWord(p,i,len,ws)
struct Lines * p;
int * i;
int * len;
char * ws;
{
int EndOfWord = 0;
int j = *i;
int l = 0;

/* remove leading blanks */

while ( (p != NULL) && strchr(ws,p-> Literal[j]) )
   if (p-> Literal[j] == 0)
      {
      if (p-> Literal[j-1] == '\\')
         {
         p = p-> next;
         j = 0;
         }
      else
         break;
      }
   else
      j++;

if (p == NULL)
   return NULL;

while (!EndOfWord)
   {
   switch(p-> Literal[j])
      {
      case '\\':
                j = 0;
                p = p-> next;
      case 0:
      case ' ':
      case '\t':
               buffer[l] = 0;
               l++;
               *i = j;
               *len = l;
               EndOfWord = 1;
               return p;
      default:
               buffer[l] = p-> Literal[j];
               l++;
               j++;
               break;
      }
   }

} /* end of ExtractWord */
void DeleteUnreferencedVariables()
{
struct Lines * p;
int notdone = 1;
int pass = 1;

while (notdone)
   {

   notdone = 0;
   for (p = Start; p != NULL; p = p-> next)
      {
      if (p-> UseFlag == pass && strchr(p-> Literal,'$'))
         notdone += TestForVars(p,pass+1);
      }
   if (DebugFlag)
      fprintf(stderr,"Pass %d yields %d transition(s)\n",pass,notdone);
   pass++;
   }

} /* end of DeleteUnreferencedVariables */
TestForVars(p,u)
struct Lines * p;
int u;
{
struct Lines * v;
struct Lines * prev = StartVar;
int retval = 0;

for (v = StartVar; v != NULL; v = v-> nextvar)
   {
   if (v-> UseFlag == QUESTIONABLE && index(v-> Variable, p-> Literal))
      {
      MarkLogical(v,u);
      if (v == StartVar)
         StartVar = v-> nextvar;
      else
         prev-> nextvar = v-> nextvar;
      retval++;
      }
   prev = v;
   }

return retval;

} /* end of TestForVars */
index(s1,s2)
char * s1;
char * s2;
{
register i = 0;
int s1len = strlen(s1);

for (i = 0; i <= strlen(s2) - s1len; i++)
   if (!strncmp(s1,&s2[i],s1len))
      return 1;

return 0;

} /* end of index */
void WriteFile()
{
struct Lines * p;

for (p = Start; p != NULL; p = p-> next)
   if (p-> UseFlag > 0)
      printf("%s%s\n",(p-> TabFlag ? "\t" : ""),
            &p-> Literal[p-> StartChar]);

} /* end of WriteFile */
