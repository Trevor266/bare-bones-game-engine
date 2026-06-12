#include<stdio.h>
#include <stdlib.h> 
#include "file.h"

/*
    // A note on C standard library I/O
    C provides mechanisms in the runtime that allow simple function calls to handle platform specific file i/o operations abstractly.
    The drawback of using the C runtime is there is no provided async i/o implementation, this requires platform specific handling. 
    The C runtime can open multiple file instances at once, but the runtime calls that work with them are blocking calls, so though they are thread safe, 
    they will block the calling thread. This in essence results in only a single active file being read at any given time, even if the system is swapping threads,
    so in essence you can think of the C runtime as only able to work with a single file at any given time, even if it tracks multiple file states at once.
    The runtime allows us to request a file handle from the operating system, abstracted into a FILE handle. This handle is an opaque handle to the c runtimes 
    interally tracked file state. This handle must be kept track of and closed out manually or the C runtime will hold onto it. 
    The runtime further provides abstractions for reading bytes and writing bytes 
    to files. To do this, it provides us a "cursor" mechanism internally for each file context, this can be thought of as a simple index:

    [0][1][2][3][4][5][6][7][8][9][10]
        ^     <-- C Runtime Cursor pointing at byte 2.

    The C runtime expects us to manually move this cursor ourselves. This provides the ability to get the size of a file, which can be used to provide 
    a read mechanism the size of the buffer needed to read out of the file.

    NOTE: This engine for the time being will handle file io via C runtime exclusively, so async file i/o will not be supported up front until the need arises to
    handle OS level file i/o. 
*/

char *ReadTextFile(const char *path)
{
    // Get C runtime file handle.
    FILE *file = fopen(path, "rb");

    if (!file) 
    {
        return NULL;
    }

    // Move the file cursor to the end of the file to get it's size, then reset the cursor.
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // File size + 1 for null terminator.
    char *buffer = malloc(fileSize + 1); 
    if (!buffer)
    {
        // Hygiene if we somehow aren't given memory.
        fclose(file);
        return NULL;
    }

    // Read the file contents into the buffer and manually set null terminator.
    fread(buffer, fileSize, 1, file);
    buffer[fileSize] = '\0';

    // Close out file.
    fclose(file);

    // NOTE: Caller is responsible for calling free on this.
    return buffer;
}