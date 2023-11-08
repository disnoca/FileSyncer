#ifndef DIRECTORY_EVENT_LISTENER_H_INCLUDED
#define DIRECTORY_EVENT_LISTENER_H_INCLUDED

#include <windows.h>

#define FILE_ACTION_MOVED 	0x00000006
#define FILE_ACTION_RENAMED FILE_ACTION_RENAMED_OLD_NAME

typedef struct {
	DWORD action;
	WCHAR *dirPath;
	WCHAR *fileName, *oldFileName;
} DirectoryEvent;

void testHandlerFunction(DirectoryEvent de);

/**
 * Starts a directory event listener for the specified directory.
 * 
 * Assumes that given dirPath was allocated and won't be freed.
 * 
 * The struct passed to the handler function's members fileName and, in case it's not null, oldFileName must be freed after their use.
 * 
 * @param dirPath the path to the directory to listen to
 * @param handlerFunction the function to call when a directory event occurs
*/
void StartDirectoryEventListener(WCHAR* dirPath, void (*handlerFunction) (DirectoryEvent de));

#endif
