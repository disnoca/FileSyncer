#include "wrapper_functions.h"
#include "directory_event_listener.h"
#include "data_structures/queue/queue.h"

#define LISTENER_LOOP_BUFFER_SIZE ((3 * sizeof(DWORD) + MAX_PATH * sizeof(WCHAR)) * 4)

#define DISPATCHER_LOOP_NO_EVENTS_SLEEP_TIME 1000
#define DISPATCHER_LOOP_EVENT_BATCH_CHECK_SLEEP_TIME 100


typedef struct {
	WCHAR* dirPath;
	void (*handlerFunction) (DWORD action, WCHAR* dirPath, WCHAR* filename);
	Queue* eventQueue;
} DirectoryEventListenerData;


/**
 * Processes the saved directory events and calls the handler function.
 * 
 * @param lpParam the DirectoryEventListenerData
*/
static DWORD WINAPI DirectoryEventDispatcherLoop(LPVOID lpParam) {
	DirectoryEventListenerData *deld = (DirectoryEventListenerData*) lpParam;

	while(TRUE) {
		while(deld->eventQueue->length == 0)
			Sleep(DISPATCHER_LOOP_NO_EVENTS_SLEEP_TIME);

		FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*) ConcurrentDequeue(deld->eventQueue);
		FILE_NOTIFY_INFORMATION* currFni = fni;

		while(TRUE) {
			WCHAR* filename = Malloc(currFni->FileNameLength + sizeof(WCHAR));
			memcpy(filename, currFni->FileName, currFni->FileNameLength);
			filename[currFni->FileNameLength/2] = L'\0';

			switch(currFni->Action) {
				case FILE_ACTION_ADDED: printf("Added: %ls\n", filename); break;
				case FILE_ACTION_REMOVED: printf("Removed: %ls\n", filename); break;
				case FILE_ACTION_MODIFIED: printf("Modified: %ls\n", filename); break;
				case FILE_ACTION_RENAMED_OLD_NAME: printf("Renamed old name: %ls\n", filename); break;
				case FILE_ACTION_RENAMED_NEW_NAME: printf("Renamed new name: %ls\n", filename); break;
			}

			// handlerFunction(currFni->Action, deld->dirPath, filename);
			Free(filename);

			if(currFni->NextEntryOffset == 0)
				break;

			*((BYTE**)&currFni) += currFni->NextEntryOffset;
		}

		Free(fni);
	}

	return 0;
}

/**
 * Listens and saves events in the specified directory indefinitely.
 * 
 * @param lpParam the DirectoryEventListenerData
*/
static DWORD WINAPI DirectoryEventListenerLoop(LPVOID lpParam) {
	DirectoryEventListenerData *deld = (DirectoryEventListenerData*) lpParam;

	HANDLE hDir = _CreateFileW(deld->dirPath, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	BYTE buffer[LISTENER_LOOP_BUFFER_SIZE];
	OVERLAPPED overlapped;
	DWORD bytesTransferred;
  	overlapped.hEvent = _CreateEventW(NULL, FALSE, 0, NULL);

	while(TRUE) {
		// Wait for changes in the directory
		_ReadDirectoryChangesW(hDir, buffer, LISTENER_LOOP_BUFFER_SIZE, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL);
		_WaitForSingleObject(overlapped.hEvent, INFINITE);
		_GetOverlappedResult(hDir, &overlapped, &bytesTransferred, FALSE);

		// Save and enqueue the changes
		FILE_NOTIFY_INFORMATION* fni = Malloc(bytesTransferred);
		memcpy(fni, buffer, bytesTransferred);
		ConcurrentEnqueue(deld->eventQueue, fni);
	}

	return 0;
}

void StartDirectoryEventListener(WCHAR* dirPath, void (*handlerFunction) (DWORD action, WCHAR* dirPath, WCHAR* filename)) {
	DirectoryEventListenerData *deld = Malloc(sizeof(DirectoryEventListenerData));
	deld->dirPath = dirPath;
	deld->handlerFunction = handlerFunction;
	deld->eventQueue = QueueCreate();

	_CreateThread(NULL, 0, DirectoryEventListenerLoop, deld, 0, NULL);
	_CreateThread(NULL, 0, DirectoryEventDispatcherLoop, deld, 0, NULL);
}
