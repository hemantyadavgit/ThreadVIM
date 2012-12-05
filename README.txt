/*Copyright (C) 2012 Hemant Yadav

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

Program purpose:
This program is a multi-threaded text editor. It works similarly to a vim like text editor. It can modify text files by inserting text, deleting text, shifting lines up or down as needed when the text is modified, and is able to auto-save along with regular saving.

Compilation:
The program can be compiled by navigating to the directory that contains the source files from a terminal and using the included makefile. When in the correct directory, type the command "make clean" in the terminal to remove the current compiled files. Next type the command "make all" to compile the program and to create the executable editor file.

Running the program:
To run the program from the terminal navigate into the directory containing the executable editor file and type the command "./editor filler.txt" to run the program. The first part of the command is the executable program file and the second part is the name of the document you would like to edit, which is filler.txt in the example.

What the program does:
The program starts out by creating the router, UI and autosave threads. The router thread is responsible for taking in messages from a message queue and performing the actions requested by the messages. Each of the messages contains a command which could be EDIT, DEL, QUIT, or SAVE. The UI thread is responsible for passing commands to the router thread which could include save, quit, insert and delete. The autosave thread is responsible for saving a text buffer to a temporary file. The text buffer, textbuff, is responsible for holding the text from the open text file. The lines of text are stored in the buffer as nodes in a linked list.

Contribution:
Hemant started the project and did design and programming. Andrew did makefile and README.