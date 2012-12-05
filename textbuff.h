/*Copyright (C) 2012 Hemant Yadav

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
#ifndef textbuff_h_
#define textbuff_h_


// Maximum length of a line in the textbuffer
#define LINEMAX 100

struct node_t{
    char *data;
    struct node_t *next;
} typedef node;

    /**
     * Initializes the required data structures necessary
     * for the text buffer. Additionally it opens and reads
     * file into the text buffer.
     * @params file the file to be opened and read into
     * the text buffer.
     */
    void init_textbuff(char* file);

    /**
     * Appends the everything from line
     * until a newline is reached to the
     * current text buffer. If LINEMAX is
     * reached it inserts a newline and
     * continues appending line.
     * @params line the line of text to be
     * appended
     * @returns 0 if error occurs and 1 if successful
     */
    int appendLine(char* line);

    /**
     * Fetches line index from the buffer
     * and places it in returnLine
     * @params index line to fetch
     * returnLine pointer which will point to
     * a copy of the desired line. Memory allocated
     * with malloc.
     * @returns 0 if error occurs or 1 if successful
     */
    int getLine(int index, char** returnLine);

    /**
     * Inserts text into row at position col.
     * If col is zero insert at the begining of the line.
     * If the new line length is more than LINEMAX
     * then split the line at line max and insert a
     * new line in the text buffer after the current line.
     * @returns 0 if error occurs or 1 if successful
     */
    int insert(int row, int col, char text);

    /**
     * Returns the number of lines in the textbuffer
     */
    int getLineLength();

    /**
     * Delete the line index
     * @returns 0 if error otherwise returns 1
     */
    int deleteLine(int index);

    /**
     * Delete a single characer at (row,col)
     * from the text buffer
     * @returns 0 if error otherwise returns 1
     */
    int deleteCharacter(int row, int col);

    /**
     * Cleans up the buffer sets the buffer length
     * to -1 represinting that the buffer is invalid
     */ 
    void deleteBuffer();
#endif