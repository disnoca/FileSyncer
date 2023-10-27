#ifndef DIRECTORY_EVENT_LISTENER_H_INCLUDED
#define DIRECTORY_EVENT_LISTENER_H_INCLUDED

#include <windows.h>

/**
 * Starts a directory event listener for the specified directory.
 * 
 * The file_name parameter passed to the handler function must be freed after its use.
 * 
 * @param dir_path the path to the directory to listen to
 * @param handler_function the function to call when a directory event occurs
*/
void directory_event_listener_start(WCHAR* dir_path, void (*handler_function) (DWORD action, WCHAR* dir_path, WCHAR* file_name));

#endif
