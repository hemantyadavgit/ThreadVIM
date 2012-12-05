/*Copyright (C) 2012 Hemant Yadav

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

//Cover all the bases
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "textbuff.h"
#define LINEMAX 100

node *first; //First node of the list
//node *last; //Last node of the list
/**
     * Initializes the required data structures necessary
     * for the text buffer. Additionally it opens and reads
     * file into the text buffer.
     * @params file the file to be opened and read into
     * the text buffer.
     */
void init_textbuff(char *file) //Initializes data structures
{
 
 char str[LINEMAX+2]; //For handling newlines and special characters
 FILE *handle = NULL;
 node *last = NULL;
 //node *fend = NULL;
 
 //Open file in RW mode
 if(!(handle=fopen(file,"rw")))
 {
  fprintf(stderr, "Unable to open file! \n");
  exit(1);
 }
 
 //If file is read, setup list
 while(fgets(str, LINEMAX + 5, handle))
 {
  node *fend = (node*)malloc(sizeof(node));
       if(!fend)
                {
                 fprintf(stderr, "malloc() failed in init_textbuff\n");
                 exit(1);
                }
       else if(!last) 
                {
                 first = fend;
                 last = fend;
                }
       else
                {
                 last->next=fend;
                 last=fend;
                }
                
       last->data=malloc(LINEMAX+5);
       
       if(!last->data)
               {
                fprintf(stderr,"malloc() failed for last->data");
                exit(1);
               }
       if (strlen(str)==LINEMAX+1&&str[LINEMAX]!='\n')
          ungetc(str[LINEMAX], handle);
       if (strlen(str))
           str[strlen(str)-1]='\0';
           strncpy(last->data, str, LINEMAX+1);
  }
 //In case file is empty insert null string
  if(!first)
  {
            if(!(first=malloc(sizeof(node)))||!(first->data=malloc(sizeof(char))))
            {
             fprintf(stderr, "malloc() failed! (Empty File Case)!");
             exit(1);
            }
            first->data[sizeof(char)-sizeof(char)]='\0';
  }
};
   /**
     * Fetches line index from the buffer
     * and places it in returnLine
     * @params index line to fetch
     * returnLine pointer which will point to
     * a copy of the desired line. Memory allocated
     * with malloc.
     * @returns 0 if error occurs or 1 if successful
     */            
int getLine(int index, char** returnLine)
{
    int index_count = 0;
    node *last = first;
    while(last&&index_count<index)
    {
       last=last->next;
       index_count++;
    }                 
    if(!last)
             return 0;
    
        *returnLine=last->data;
        return 1;
 
}
/**
     * Inserts text into row at position col.
     * If col is zero insert at the begining of the line.
     * If the new line length is more than LINEMAX
     * then split the line at line max and insert a
     * new line in the text buffer after the current line.
     * @returns 0 if error occurs or 1 if successful
     */
int insert(int row, int col, char text)
{
 if(!first)
          return 0;
 node *last = first;
 node *temp=NULL;
 int row_count=0;
 while(last&&row_count<row)
 {
    temp=last;
    last=last->next;
    row_count++;
 }
 if(!last&&row_count==row)
 {
   node *new_line;
   if(!(new_line=malloc(sizeof(node))))
   {
    fprintf(stderr, "malloc() failed!\n");
   }
   //new_line->data=malloc(sizeof(text)+1);
   //strcpy(new_line->data, text);
   //strcat(new_line->data,'\0');
   new_line->data[0] = text;
   new_line->data = '\0';
   new_line->next = NULL;
   temp->next = new_line;
  }
  int str_len = strlen(last->data);
  //int str_len=0;
  //str_len++;
  //handle different cases here
  if(!last||col>LINEMAX||(col==LINEMAX||col>str_len))
  {//fprintf(stderr, "here");
               return 0;}
 if(col==LINEMAX-1&&str_len==LINEMAX-1)
 
       return insert(row+1, 0, text);
  else if (col==str_len)
  {
       last->data[col]=text;
       last->data[col+1]='\0';
       return 1;
  }
  else if (str_len==LINEMAX)//when str_len hits LINEMAX, jump to next line!
  {
       char c=last->data[str_len-1];
       insert(row+1,0,c);
       str_len--;
  }
  last->data[str_len+1]='\0';
  for(row_count=str_len;row_count>col;row_count--)
  {
   last->data[row_count]=last->data[row_count-1];
  }
  last->data[col]=text;
  return 1;
};
/**
     * Returns the number of lines in the textbuffer
     */
int getLineLength()
{
 int line_count=0;
 node *last=first;
      while(last)
       {
        last=last->next;
        line_count++;
       }
 return line_count;
};
/**
     * Delete a single characer at (row,col)
     * from the text buffer
     * @returns 0 if error otherwise returns 1
     */
int deleteCharacter(int row, int col)
{
   int row_count=0;
    node *last=first;
    while(last&&row_count<row)
    {
     last=last->next;
     row_count++;
    }
    if(!last)
            return 0;
    int len = strlen(last->data);
    if(col>=0&&col<len&&col<LINEMAX)
    {
                   for(row_count=col;row_count<len;row_count++)
                   {
                      last->data[row_count]=last->data[row_count+1];
                   }
                   return 1;
    }
return 0;

};
/**
     * Cleans up the buffer sets the buffer length
     * to -1 represinting that the buffer is invalid
     */
void deleteBuffer(){
    while(first)
    {//walk the list and free memory
     node *walk=first;
     first=first->next;
     free(walk->data);
     free(walk);
    }
};

   
   
   
 
    
  
 
 
 