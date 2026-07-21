#include "../include/file.h"

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

// Passes back a char* containing the absolute path minus the executable file
// outSize should be the sizeof(outPath)
// Relative path is the path you'd like to append to the exe directory, if any. This can be a relative path.
void GetRelativePathFromExecutableDirectory(char *outPath, size_t outSize, const char *relativePath)
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

void GetFilesContainingFolder(char *filePathBuffer, char *containingFolderPathBuffer)
{
    /*
        Paths can use either back or forward slashes, and various systems tolerate various mixed usages of these, 
        therefore this function assumes it may receive either/or. We need to determine which type of slash 
        the path ends on. The idea is we are going to take a path like this one:

        path/to/file/plus/thefile.name

        We need to find the last slash, and then determine it's size:
        "path/to/file/plus/" split off"thefile.name".

        Once we do this, we know the size of the buffer we need to allocate, so we can take the index of our last slash to get the file path 
        to copy into the outPath
    */
    char *lastBackslash = strrchr(filePathBuffer, '\\');
    char *lastForwardSlash = strrchr(filePathBuffer, '/');
    
    // Whichever of these has the large value (memory address) can be assumed to further down the string than the other, so we set the slash here.
    char *lastSlash = lastForwardSlash > lastBackslash ? lastForwardSlash : lastBackslash;

    if (lastSlash != NULL)
    {
        // Once we find the last slash, use it's memory address to find where it is relative to the file path string, 
        // and get the size difference between the two, add one because the slash position itself is not accounted for so this will be one off.
        size_t filePathSizeWithoutFile = (size_t)(lastSlash - filePathBuffer) + 1;
        memcpy(containingFolderPathBuffer, filePathBuffer, filePathSizeWithoutFile);
        containingFolderPathBuffer[filePathSizeWithoutFile] = '\0';
    }
    else
    {
        containingFolderPathBuffer[0] = '\0';
    }
}

void AppendFilePath(char *dest, size_t destSize, const char *basePath, const char *appendedPath)
{
    snprintf(dest, destSize, "%s%s", basePath, appendedPath);
}

// TODO: I think this should return back a char* and require the caller to cast.
BOOL GetCanonicalizedExecutableWorkingDirectory(wchar_t *outPath, size_t outSize, const char *relativePath)
{
    char rawPath[MAX_OS_DIRECTORY_LENGTH];
    GetRelativePathFromExecutableDirectory(rawPath, sizeof(rawPath), relativePath);

    wchar_t wRawPath[MAX_OS_DIRECTORY_LENGTH];
    if (MultiByteToWideChar(CP_UTF8, 0, rawPath, -1, wRawPath, MAX_PATH) == 0)
    {
        return FALSE;
    }

    wchar_t wCanonicalPath[MAX_OS_DIRECTORY_LENGTH];
    DWORD canonResult = GetFullPathNameW(wRawPath, MAX_OS_DIRECTORY_LENGTH, wCanonicalPath, NULL);
    if (canonResult == 0 || canonResult > MAX_OS_DIRECTORY_LENGTH)
    {
        return FALSE;
    }

    wcsncpy(outPath, wCanonicalPath, outSize / sizeof(wchar_t));
    outPath[outSize / sizeof(wchar_t) - 1] = L'\0';

    return TRUE;
}

BOOL BuildPIDLISTFromPath(wchar_t *directoryPath, PIDLIST_ABSOLUTE *pidlAbsolute)
{
    HRESULT folderTreeRootFolderResult = SHParseDisplayName(directoryPath, NULL, pidlAbsolute, 0, NULL);
    if (folderTreeRootFolderResult != S_OK)
    {
        return FALSE;
    }

    return TRUE;
}