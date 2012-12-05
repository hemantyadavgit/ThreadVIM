/*Copyright (C) 2012 Hemant Yadav

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "textbuff.h"
#include <unistd.h> 

#define CTRL(c) ((c) & 037)

#define min(X, Y)                \
     ({ typeof (X) x_ = (X);          \
        typeof (Y) y_ = (Y);          \
        (x_ < y_) ? x_ : y_; })


// Defined constants to be used with the message data-structure
#define EDIT 0
#define SAVE 1
#define QUIT 2
#define DEL  3
#define SLEEP_TIME 5
// Maximum length of the message passing queue
#define QUEUEMAX 20
//Globals
int global_count;
char *global_fname;


// Data structure for message passing queue used to communicate
// between the router and UI thread
struct message_t{
    int data;
    int row;
    int col;
    int command;
    struct message_t *next;
} typedef message;
message *first_message=NULL;
message *last_message=NULL;

// mutex to control access to the text buffer
pthread_mutex_t text_ = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t front_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t back_lock = PTHREAD_MUTEX_INITIALIZER;


// The current position of the cursor in the screen
int row;
int col;

// Lines visible in the current view of textbuff
// visible on the screen
int view_min;
int view_max;

/**
 * Removes the first message from the message queue
 * and returns it.
 */
message* pop(){
  if(first_message)
  {
   pthread_mutex_lock(&front_lock);
   message *popped_msg=first_message;
   first_message=first_message->next;
   global_count--;
   pthread_mutex_unlock(&front_lock);
   return popped_msg;
  }
  else
      return NULL;
}


/**
 * Inserts a message at the back of the message queue
 */
void push(message* m_){
     pthread_mutex_lock(&front_lock);
     //check if queue is full
     while(global_count>=QUEUEMAX)
     {
      pthread_mutex_unlock(&front_lock);
      sleep(SLEEP_TIME);
      pthread_mutex_unlock(&front_lock);
     }
     if(!first_message)
     {
      first_message=m_;
      last_message=m_;
      first_message->next=NULL;
     }
     else 
     {
      last_message->next=m_;
      m_->next=NULL;
     }
     global_count++;
     pthread_mutex_unlock(&front_lock);
      

}


/**
 * Redraws the screen with lines min_line -> max_line visible on the screen
 * places the cursor at (r_, c_) when done. If insert_ is 1 then "INPUT MODE"
 * text is displayed otherwise if 0 it isn't.
 */
int redraw(int min_line, int max_line,int r_, int c_, int insert_){
    erase();
    if(max_line - min_line != LINES-1){
        perror("HELP");
        pthread_exit(NULL);
    }
    move(0,0);

    pthread_mutex_lock(&text_);

    for(;min_line < max_line;min_line++){
        char *line;
        if(getLine(min_line,&line) == 0)
            break;
        int j;
        for(j=0;j < strlen(line);j++){
            addch(*(line+j));
        }
        addch('\n');
    }

    pthread_mutex_unlock(&text_);

    if(insert_){
        standout();
        mvaddstr(LINES-1, COLS-20, "INPUT MODE");
        standend();
    }

    move(r_,c_);
    refresh();
    return 1;
}


/**
 * Input loop of the UI Thread;
 * Loops reading in characters using getch() and placing them into the textbuffer using message queue interface
 */
void input_mode(){
    int c;
    redraw(view_min, view_max, row, col, 1);
    refresh();
    for(;;){
        c = getch();
        if(c == CTRL('D')){
            break;
        }

        //int insert_row = row+view_min;
        //int insert_col = col;

        //Add code here to insert c into textbuff at (insert_row, insert_col) using the message queue interface.
        message temp;
        temp.row=row;
        temp.col=col;
        temp.command=EDIT;
        temp.data=c;
        push(&temp);

        // ------------------------------
        if(col<COLS-1) {
            col++;
        }else{
            col = 0;
            row++;
            }
            if(row > LINES - 1){
                view_min++;
                view_max++;
            }
        
        redraw(view_min,view_max,row,col,1);
    }
    redraw(view_min,view_max,row,col,0);
}


/**
 * Main loop of the UI thread. It reads in commands as characters
 */
void loop(){
    int c;

    while(1){
        move(row,col);
        refresh();
        c = getch();
        message temp;
        switch(c){
            case 'h':
            case KEY_LEFT:
                if(col > 0)
                    col--;
                else
                    flash();
                break;
            case 'j':
            case KEY_DOWN:
                if(row < LINES -2)
                    row++;
                else
                    if(view_max+1<=getLineLength())
                        redraw(++view_min,++view_max,row,col,0);
                    else
                        flash();
                break;
            case 'k':
            case KEY_UP:
                if(row > 0)
                    row--;
                else
                    if(view_min-1 > -1)
                        redraw(--view_min,--view_max,row,col,0);
                    else
                        flash();
                break;
            case 'l':
            case KEY_RIGHT:
                if((col<COLS-1) && (col<LINEMAX-1))
                    col++;
                else
                    flash();
                break;
            case 'i':
            case KEY_IC:
                input_mode();
                break;
            case 'x':
                flash();

                //int del_row = row+view_min;
                //int del_col = col;

                // Add code here to delete character (del_row, del_col) from textbuf

                // ------------------------------
                //message *temp;
                temp.row=row;
                temp.col=col;
                temp.command=DEL;
                push(&temp);

                redraw(view_min,view_max,row,col,0);
                break;
            case 'w':
                flash();

                // Add code here to save the textbuf file


                // ------------------------------
                temp.command=SAVE;
                push(&temp);

                break;
            case 'q':
                endwin();

                // Add code here to quit the program

                // ------------------------------
                temp.command=QUIT;
                push(&temp);
                pthread_exit(NULL);
            default:
                flash();
                break;
        }

    }
}


/**
 * Function to be used to spawn the UI thread.
 */
void *start_UI(void *threadid){

    initscr();
    cbreak();
    nonl();
    noecho();
    idlok(stdscr, TRUE);
    keypad(stdscr,TRUE);

    view_min = 0;
    view_max = LINES-1;

    redraw(view_min,view_max,row,col,0);

    refresh();
    loop();
    return NULL;
}

/**
 * Function to be used to spawn the autosave thread.
 */
void *autosave(void *threadid){

    // This function loops until told otherwise from the router thread. Each loop:
    while(TRUE)
    {
               pthread_mutex_lock(&back_lock);
        // Open the temporary file
                FILE *handle_temp;
                if(!(handle_temp=fopen(global_fname,"w")))
                {
                 fprintf(stderr,"Global Filename : %s",global_fname);
                 fprintf(stderr,"File write failed!\n");
                 pthread_mutex_unlock(&back_lock);
                 continue;
                }
        // Read lines from the text buffer and save them to the temporary file
           pthread_mutex_lock(&text_);
           int c, numlines;
           numlines=getLineLength();
           for(c=0;c<numlines;c++)
           {
            char *str_e;
            if(getLine(c, &str_e))
            {
             fprintf(handle_temp,"%s",str_e);
             if(c<numlines-1)
             fprintf(handle_temp,"\n");
            }
           }
           pthread_mutex_unlock(&text_);
        // Close the temporary file and sleep for 5 sec.
        fclose(handle_temp);
        pthread_mutex_unlock(&back_lock);
        sleep(5);
        //fprintf(stderr,"woke up");
        }
    return NULL;
}       

int main(int argc, char **argv){

    row = 0;
    col = 0;

    // get text file from argv
    char *main_fname=NULL;
    if (argc==2)
       //strcpy(main_fname, argv[1]);
       main_fname=argv[1];
    else
    {
     printf("Usage : %s <file-name>",argv[0]);
     exit(0);
    }
    int len=strlen(main_fname);
    global_fname=malloc(strlen(main_fname)+2);
    //strcat(global_fname,'~');
    //strcat(global_fname,'\0');
    strcpy(global_fname, main_fname);
    global_fname[len]='~';
    global_fname[len+1]='\0';

    // set up necessary data structures for message passing queue and initialize textbuff
    first_message=NULL;
    last_message=NULL;
    global_count=0;
    init_textbuff(main_fname);

    // spawn UI thread
    pthread_t ui;
    int ui_id=pthread_create(&ui,NULL,(void*)&start_UI,NULL);
    // spawn auto-save thread
    pthread_t auto_save;
    int auto_save_id=pthread_create(&auto_save,NULL,(void*)autosave,NULL);
    // Main loop until told otherwise
    int isTrue=1;
    while(isTrue)
    {
     
        // Recieve messages from the message queue
        message *msg_l = pop();
        if (!msg_l) 
           continue;
        switch(msg_l->command)
        {
        // If EDIT then place the edits into the text buffer
        case EDIT:
        pthread_mutex_lock(&text_);
        insert(msg_l->row,msg_l->col,msg_l->data);
        pthread_mutex_unlock(&text_);
        break;
        // If SAVE then save the file additionally delete the temporary save
        case SAVE:
        {
        FILE *handle_switch;
        if(!(handle_switch=fopen(main_fname,"w")))
        {
        fprintf(stderr," Save failed!");continue;
        }
        int c, numlines;
        numlines=getLineLength();
        for(c=0;c<numlines;c++)
        {
         char *temp_line;
         if(getLine(c,&temp_line))
         {
          fprintf(handle_switch,"%s",temp_line);
          if(c<numlines-1)
          fprintf(handle_switch,"\n");
         }
        }
        fclose(handle_switch);
        pthread_mutex_lock(&back_lock);
        unlink(global_fname); //delete the temporary file!
        pthread_mutex_unlock(&back_lock);}
        break;
               
        // If QUIT then quit the program and tell the appropriate threads to stop
        case QUIT:
        isTrue=0;
        break;
        case DEL:
        // If DEL then delete the specified character from the text buffer
        pthread_mutex_lock(&text_);
        deleteCharacter(msg_l->row,msg_l->col);
        pthread_mutex_unlock(&text_);
        break;
    // Clean up data structures
    }
}
//clean up memory
printf("\nExiting....Doing a little housekeeping\n");
pthread_cancel(auto_save_id);
pthread_join(auto_save_id,NULL);
pthread_join(ui_id,NULL);
free(global_fname);
deleteBuffer();
while(pop());
printf("Housekeeping done! Goodbye!");
return 0;
}