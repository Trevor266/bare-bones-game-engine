#include<stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <windows.h>
#include "../include/file.h"
#include "../include/bitmap.h"

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
    FILE *fileHandle = GetFileHandle(path);

    if (!fileHandle) 
    {
        return NULL;
    }

    const long fileSize = GetFileBufferSize(fileHandle);

    // File size + 1 for null terminator.
    char *buffer = malloc(fileSize + 1); 
    if (!buffer)
    {
        // Hygiene if we somehow aren't given memory.
        fclose(fileHandle);
        return NULL;
    }

    // Read the file contents into the buffer and manually set null terminator.
    fread(buffer, fileSize, 1, fileHandle);
    buffer[fileSize] = '\0';

    // Close out file.
    fclose(fileHandle);

    // NOTE: Caller is responsible for freeing on this.
    return buffer;
}

Bitmap *ReadBitmapFromFile(const char *path)
{
    // Get C runtime file handle.
    FILE *fileHandle = GetFileHandle(path);
    if (!fileHandle)
    {
        return NULL;
    }

    // Use the file handle to read the bitmap header. The BitmapFileHeader struct should map 1:1 with 
    // a bitmap file header. We first read the first byte of the file and read the size of the bitmap header.
    // If this read succeeds, we then need to check the signature, compression methods, and bits per pixel to 
    // verify this is a bitmap we can work with.
    // Signature is valid when the value is 'BM'.
    // Compression methods 0 and 3 mean RGB and RGB+alpa (RGBA).
    // This supports a 24 or 32 bit bitmap.
    BitmapFileHeader header;
    bool headerReadSucceeded = fread(&header, sizeof(BitmapFileHeader), 1, fileHandle) == 1;
    bool validHeaderSignature = header.signature == 0x4D42;
    bool validBmpCompression = header.compression == 0 || header.compression == 3;
    bool validBitsPerPixel = header.bitsPerPixel == 24 || header.bitsPerPixel == 32;
    
    // If any of these checks fail, we definitely don't have a valid bitmap, bail (rhyme).
    if (!headerReadSucceeded || !validHeaderSignature || !validBmpCompression || !validBitsPerPixel)
    {
        fclose(fileHandle);
        return NULL;
    }

    /*
        Next get the relevant bitmap data. We need the height to be abs() because bitmaps either store the height in the header as 
        positive if the pixels are laid out bottom to top, or negative if the pixels are laid out top to bottom.
        We need to determine the size of a row of pixel data in the bitmap to properly read it. 
        Bitmaps always pad pixel data to the nearest interval of 4. Because of this, we check the pixel width of the bitmap 
        against the channel count, and round up to the nearest interval of 4 to get the size of the row.

        Diagram for a 2 pixel wide 3 channel row layout

        Pixel 1          Pixel 2          Row Padding
        [B][G][R]        [B][G][R]        [pad][pad]
        0   1   2        3   4   5        6     7

        Padding is added to the end of the row to align it to the nearest interval of 4. If we had a 4 channel bitmap, the padding would not be needed 
        because the extra 2 bytes would be used for the alpha channel instead of padding. In this sense you can think of it like 3 channel bitmaps may or 
        may not include padding depending on how they end up aligning, and 4 channel bitmaps never will use padding as they always align on 4.
    */
    const int       width    = header.width;
    const int       height   = abs(header.height);
    const int       channels = header.bitsPerPixel / 8; // bits to bytes
    const int       rowSize  = (width * channels + 3) & ~3; 
    const int       padding = rowSize - (width * channels);
    const uint8_t   isTopDown = header.height < 0;

    // Allocate memory for a bitmap plus the pixel data size, we can then +1 bitmap to access the pixel data portion of this memory.
    Bitmap *bitmap = malloc(sizeof(Bitmap) + (rowSize * height));
    if (!bitmap)
    {
        fclose(fileHandle);
        return NULL;
    }
    
    // Set pixel pointer to upper portion of previously allocated memory, and set other bitmap information.
    bitmap->pixels = (uint8_t *)(bitmap + 1);
    bitmap->width    = width;
    bitmap->height   = height;
    bitmap->channels = channels;

    // Set the file cursor to the pixel data offset to get the first pixel position. If this fails, stop.
    if (fseek(fileHandle, header.pixelOffset, SEEK_SET)) 
    {
        free(bitmap);
        fclose(fileHandle);
        return NULL;
    }

    for (int row = 0; row < height; row++)
    {
        // Based on the orientation of the bitmap, get the next row number.
        int nextRowNumber  = isTopDown ? row : (height - 1 - row);

        // Get a pointer to the next chunk of row data out of our bitmap structure based on the current iteration.
        uint8_t *nextRow = bitmap->pixels + nextRowNumber * width * channels;

        // Attempt to write the next chunk of pixel data into the bitmap pixel buffer, if we fail to do this, we cannot guarantee validity of the bitmap, bail out.
        if (fread(nextRow, width * channels, 1, fileHandle) != 1)
        {
            free(bitmap);
            fclose(fileHandle);
            return NULL;
        }

        // Now move the file io cursor to account for any potential padding.
        if (padding)
        {
            fseek(fileHandle, padding, SEEK_CUR);
        }
            

        // We convert BGR(A) to RGB(A) here, because bitmaps use BGR(A). 
        for (int col = 0; col < width * channels; col += channels)
        {
            uint8_t b = nextRow[col];
            uint8_t g = nextRow[col + 1];
            uint8_t r = nextRow[col + 2];

            // Set the next row's r/g/b components accordingly by offsetting from the column.
            nextRow[col]     = r;
            nextRow[col + 1] = g;
            nextRow[col + 2] = b;
        }
    }

    fclose(fileHandle);
    return bitmap;
}

static FILE *GetFileHandle(const char *path)
{
    return fopen(path, "rb");
}

static long GetFileBufferSize(FILE *fileHandle)
{
    // Move the file cursor to the end of the file to get it's size, then reset the cursor.
    fseek(fileHandle, 0, SEEK_END);
    long fileSize = ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_SET);

    return fileSize;
}

int CreateNewLevelFolder(const char *directoryRootPath)
{
    char basePath[MAX_PATH];
    GetExecutableWorkingDirectory(basePath, sizeof(basePath), LEVEL_PATH);
    return CreateDirectoryA(directoryRootPath, NULL) == TRUE;
}

// Passes back a char* containing the absolute path minus the executable file
// outSize should be the sizeof(outPath)
// Relative path is the path you'd like to append to the exe directory, if any.
void GetExecutableWorkingDirectory(char *outPath, size_t outSize, const char *relativePath)
{
    // We need to get the absolute path to the exe. To do this, we get the exe file path (root of main exectuable + file name), 
    // this gives us a memory address to a string that looks something like "path\\toexe\\main.exe", we want everything prior to main.exe,
    // so before we return this path, we null terminate the character directly after the lastSlash (lastSlash[1]), which is a shorthand way of 
    // eliminating all elements after a given memory address offset. This updates the original char array to only contain the relative path to the exe, without the exe itself.
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    char *lastSlash = strrchr(exePath, '\\');

    if (lastSlash != NULL) 
    {
        lastSlash[1] = '\0';
    }

    snprintf(outPath, outSize, "%s%s", exePath, relativePath);
}