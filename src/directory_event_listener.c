#include "wrapper_functions.h"
#include "directory_event_listener.h"


typedef struct {
	WCHAR* dir_path;
	void (*handler_function) (DWORD action, WCHAR* dir_path, WCHAR* file_name);
} directory_event_listener_data;


/**
 * Listens indefinitely for changes in the specified directory and calls the handler function when a change occurs.
 * 
 * @param lpParam the directory_event_listener_data struct
*/
static DWORD WINAPI directory_event_listener_thread(LPVOID lpParam) {
	directory_event_listener_data *deld = (directory_event_listener_data*) lpParam;

	HANDLE hDir = _CreateFileW(deld->dir_path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	BYTE buffer[1024];
	OVERLAPPED overlapped;
  	overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

	do {
		_ReadDirectoryChangesW(hDir, buffer, 1024, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL);

		_WaitForSingleObject(overlapped.hEvent, INFINITE);

		DWORD bytes_transferred;
		_GetOverlappedResult(hDir, &overlapped, &bytes_transferred, FALSE);

		FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*) buffer;
		while(TRUE) {
			WCHAR* file_name = Malloc(fni->FileNameLength + sizeof(WCHAR));
			memcpy(file_name, fni->FileName, fni->FileNameLength);
			file_name[fni->FileNameLength/2] = L'\0';

			handler_function(fni->Action, deld->dir_path, fni->FileName);

			if(fni->NextEntryOffset == 0)
				break;
			
			*((BYTE**)&fni) += fni->NextEntryOffset;
		}
	} while(TRUE);

	return 0;
}

void directory_event_listener_start(WCHAR* dir_path, void (*handler_function) (DWORD action, WCHAR* dir_path, WCHAR* file_name)) {
	directory_event_listener_data *deld = Malloc(sizeof(directory_event_listener_data));
	deld->dir_path = Malloc((wcslen(dir_path) + 1) * sizeof(WCHAR));
	deld->handler_function = handler_function;

	wcscpy(deld->dir_path, dir_path);

	_CreateThread(NULL, 0, directory_event_listener_thread, deld, 0, NULL);
}
