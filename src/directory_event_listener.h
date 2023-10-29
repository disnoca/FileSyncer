#ifndef DIRECTORY_EVENT_LISTENER_H_INCLUDED
#define DIRECTORY_EVENT_LISTENER_H_INCLUDED

#include <windows.h>

/**
 * Starts a directory event listener for the specified directory.
 * 
 * Assumes that dir_path was allocated and won't be freed.
 * 
 * The filename parameter passed to the handler function must be freed after its use.
 * 
 * @param dir_path the path to the directory to listen to
 * @param handlerFunction the function to call when a directory event occurs
*/
void StartDirectoryEventListener(WCHAR* dir_path, void (*handlerFunction) (DWORD action, WCHAR* dir_path, WCHAR* filename));

#endif
