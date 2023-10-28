#include "wrapper_functions.h"
#include "directory_event_listener.h"
#include "data_structures/queue/queue.h"

#define DIRECTORY_EVENT_LISTENER_LOOP_BUFFER_SIZE ((3 * sizeof(DWORD) + MAX_PATH * sizeof(WCHAR)) * 4)
#define DIRECTORY_EVENT_DISPATCHER_LOOP_SLEEP_TIME 1000


typedef struct {
	WCHAR* dir_path;
	void (*handler_function) (DWORD action, WCHAR* dir_path, WCHAR* file_name);
	queue* event_queue;
} directory_event_listener_data;


/**
 * Processes the saved directory events and calls the handler function.
 * 
 * @param lpParam the directory_event_listener_data
*/
static DWORD WINAPI directory_event_dispatcher_loop(LPVOID lpParam) {
	directory_event_listener_data *deld = (directory_event_listener_data*) lpParam;

	while(TRUE) {
		while(deld->event_queue->length == 0)
			Sleep(DIRECTORY_EVENT_DISPATCHER_LOOP_SLEEP_TIME);

		FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*) concurrent_dequeue(deld->event_queue);
		FILE_NOTIFY_INFORMATION* curr_fni = fni;

		while(TRUE) {
			WCHAR* file_name = Malloc(curr_fni->FileNameLength + sizeof(WCHAR));
			memcpy(file_name, curr_fni->FileName, curr_fni->FileNameLength);
			file_name[curr_fni->FileNameLength/2] = L'\0';

			switch(curr_fni->Action) {
				case FILE_ACTION_ADDED: printf("Added: %ls\n", file_name); break;
				case FILE_ACTION_REMOVED: printf("Removed: %ls\n", file_name); break;
				case FILE_ACTION_MODIFIED: printf("Modified: %ls\n", file_name); break;
				case FILE_ACTION_RENAMED_OLD_NAME: printf("Renamed old name: %ls\n", file_name); break;
				case FILE_ACTION_RENAMED_NEW_NAME: printf("Renamed new name: %ls\n", file_name); break;
			}

			// handler_function(curr_fni->Action, deld->dir_path, file_name);
			Free(file_name);

			if(curr_fni->NextEntryOffset == 0)
				break;

			*((BYTE**)&curr_fni) += curr_fni->NextEntryOffset;
		}

		Free(fni);
	}

	return 0;
}

/**
 * Listens and saves events in the specified directory indefinitely.
 * 
 * @param lpParam the directory_event_listener_data
*/
static DWORD WINAPI directory_event_listener_loop(LPVOID lpParam) {
	directory_event_listener_data *deld = (directory_event_listener_data*) lpParam;

	HANDLE hDir = _CreateFileW(deld->dir_path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	BYTE buffer[DIRECTORY_EVENT_LISTENER_LOOP_BUFFER_SIZE];
	OVERLAPPED overlapped;
	DWORD bytes_transferred;
  	overlapped.hEvent = _CreateEventW(NULL, FALSE, 0, NULL);

	while(TRUE) {
		// Wait for changes in the directory
		_ReadDirectoryChangesW(hDir, buffer, DIRECTORY_EVENT_LISTENER_LOOP_BUFFER_SIZE, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL);
		_WaitForSingleObject(overlapped.hEvent, INFINITE);
		_GetOverlappedResult(hDir, &overlapped, &bytes_transferred, FALSE);

		// Save and enqueue the changes
		FILE_NOTIFY_INFORMATION* fni = Malloc(bytes_transferred);
		memcpy(fni, buffer, bytes_transferred);
		concurrent_enqueue(deld->event_queue, fni);
	}

	return 0;
}

void directory_event_listener_start(WCHAR* dir_path, void (*handler_function) (DWORD action, WCHAR* dir_path, WCHAR* file_name)) {
	directory_event_listener_data *deld = Malloc(sizeof(directory_event_listener_data));
	deld->dir_path = dir_path;
	deld->handler_function = handler_function;
	deld->event_queue = queue_create();

	_CreateThread(NULL, 0, directory_event_listener_loop, deld, 0, NULL);
	_CreateThread(NULL, 0, directory_event_dispatcher_loop, deld, 0, NULL);
}
