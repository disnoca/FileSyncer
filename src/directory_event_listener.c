#include "wrapper_functions.h"
#include "directory_event_listener.h"
#include "data_structures/queue/queue.h"

#define FNI FILE_NOTIFY_INFORMATION

#define LISTENER_LOOP_BUFFER_SIZE 9999

#define DISPATCHER_LOOP_CHAIN_EVENTS_CHECK_SLEEP_TIME 5

#define NULL_ACTION 0x00000000



void testHandlerFunction(DirectoryEvent de) {
	printf("Action: %lu\n", de.action);
	printf("Dir path: %ls\n", de.dirPath);
	printf("File name: %ls\n", de.fileName);
	if(de.oldFileName != NULL) {
		printf("Old file name: %ls\n", de.oldFileName);
		Free(de.oldFileName);
	}
	printf("\n");

	Free(de.fileName);
}


typedef struct {
	WCHAR* dirPath;
	void (*handlerFunction) (DirectoryEvent de);
	Queue* eventQueue;
	HANDLE queueMutex, availableEventsEvent;
} DirectoryEventListenerData;


/* Helper Functions */

/**
 * Enqueues the given FNI in the queue with concurrency control and sets availableEventsEvent if the queue was empty.
 * 
 * @param deld the DirectoryEventListenerData
 * @param fni the FNI to enqueue
*/
void EnqueueFNI(DirectoryEventListenerData* deld, FNI* fni) {
	_WaitForSingleObject(deld->queueMutex, INFINITE);
	Enqueue(deld->eventQueue, fni);
	if(deld->eventQueue->length == 1)
		_SetEvent(deld->availableEventsEvent);
	_ReleaseMutex(deld->queueMutex);
}

/**
 * Dequeues the next FNI in the queue with concurrency control and resets availableEventsEvent if the queue became empty.
 * 
 * @param deld the DirectoryEventListenerData
 * 
 * @return the FNI dequeued
*/
FNI* DequeueFNI(DirectoryEventListenerData* deld) {
	if(deld->eventQueue->length > 1)
		return (FNI*) Dequeue(deld->eventQueue);

	_WaitForSingleObject(deld->queueMutex, INFINITE);
	FNI* fni = (FNI*) Dequeue(deld->eventQueue);
	if(deld->eventQueue->length == 0)
		_ResetEvent(deld->availableEventsEvent);
	_ReleaseMutex(deld->queueMutex);
	return fni;
}

/**
 * Allocates memory to the address pointed by the given pointer and copies the file name from the given FNI into it.
 * 
 * @param fni the FNI to copy the file name from
 * @param fileName the pointer to be allocated and have the file name copied into
*/
void CopyFNIFileName(FNI* fni, WCHAR** fileName) {
	*fileName = Malloc(fni->FileNameLength + sizeof(WCHAR));
	memcpy(*fileName, fni->FileName, fni->FileNameLength);
	(*fileName)[fni->FileNameLength/2] = L'\0';
}

/**
 * Returns true if the given FNI corresponds to the specified action and file name.
 * 
 * @param fni the FNI to check
 * @param action the action to check
 * @param fileName the file name to check
 * 
 * @return true if the given FNI corresponds to the specified action and file name
*/
BOOLEAN FNICorrespondsTo(FNI* fni, DWORD action, WCHAR* fileName) {
	return fni->Action == action && fni->FileNameLength/sizeof(WCHAR) == wcslen(fileName) && !memcmp(fileName, fni->FileName, fni->FileNameLength);
}

/**
 * Returns true if the given FNI's fileName is of a directory.
 * 
 * @param fni the FNI to check
 * 
 * @return true if the given FNI's fileName is of a directory
*/
BOOLEAN FNIIsDirectory(const WCHAR* dirPath, const FNI* fni) {
	size_t dirPathLength = wcslen(dirPath);
	WCHAR path[dirPathLength + fni->FileNameLength/sizeof(WCHAR) + 2];
	wcscpy(path, dirPath);
	path[dirPathLength] = L'\\';
	memcpy(path + dirPathLength + 1, fni->FileName, fni->FileNameLength);
	path[dirPathLength + fni->FileNameLength/sizeof(WCHAR) + 1] = L'\0';

	return _GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY;
}

/**
 * Returns true if the following event chain in the specified DirectoryEventListenerData corresponds to a move.
 * 
 * @param deld the DirectoryEventListenerData
 * @param fileName the name of the file reported as removed
 * 
 * @return true if the following event chain in the specified DirectoryEventListenerData corresponds to a move
*/
BOOLEAN IsFileMoveAction(DirectoryEventListenerData* deld, WCHAR* fileName) {
	if(deld->eventQueue->length < 2)
		return FALSE;

	WCHAR* fileNameNoPath = wcsrchr(fileName, L'\\');
	fileNameNoPath = (fileNameNoPath == NULL) ? fileName : fileNameNoPath + 1;
	size_t fileNameNoPathLength = wcslen(fileNameNoPath);

	// Check if the following event is a FILE_ACTION_ADD of the same file
	FNI* fileAddedFni = deld->eventQueue->head->data;
	if(fileAddedFni->Action != FILE_ACTION_ADDED)
		return FALSE;
	for(size_t i = 1; i <= fileNameNoPathLength; i++)
		if(fileAddedFni->FileName[fileAddedFni->FileNameLength/sizeof(WCHAR) - i] != fileNameNoPath[fileNameNoPathLength - i])
			return FALSE;
	if(fileAddedFni->FileNameLength/sizeof(WCHAR) > fileNameNoPathLength && fileAddedFni->FileName[fileAddedFni->FileNameLength/sizeof(WCHAR) - 1 - fileNameNoPathLength] != L'\\')
		return FALSE;

	size_t newDirectoryNameLength = fileAddedFni->FileNameLength/sizeof(WCHAR) - fileNameNoPathLength;
	newDirectoryNameLength -= newDirectoryNameLength > 0;
	size_t oldDirectoryNameLength = wcslen(fileName) - fileNameNoPathLength;
	oldDirectoryNameLength -= oldDirectoryNameLength > 0;

	if(newDirectoryNameLength == 0 && oldDirectoryNameLength == 0)
		return FALSE;
	if(newDirectoryNameLength > 0 && oldDirectoryNameLength > 0 && deld->eventQueue->length < 3)
		return FALSE;

	// If the file's new directory isn't the root, check if the following event is a FILE_ACTION_MODIFIED of the directory where it was added to
	if(newDirectoryNameLength > 0) {
		FNI* directoryFni = deld->eventQueue->head->next->data;
		if(directoryFni->Action != FILE_ACTION_MODIFIED || !FNIIsDirectory(deld->dirPath, directoryFni))
			return FALSE;
		if(directoryFni->FileNameLength/sizeof(WCHAR) != newDirectoryNameLength)
			return FALSE;
		for(size_t i = 0; i < newDirectoryNameLength; i++)
			if(directoryFni->FileName[i] != fileAddedFni->FileName[i])
				return FALSE;
	}
	
	// If the file's old directory isn't the root, check if the following event is a FILE_ACTION_MODIFIED of the directory where it was removed from
	if(oldDirectoryNameLength > 0) {
		FNI* directoryFni = (newDirectoryNameLength == 0) ? deld->eventQueue->head->next->data : deld->eventQueue->head->next->next->data;
		if(directoryFni->Action != FILE_ACTION_MODIFIED || !FNIIsDirectory(deld->dirPath, directoryFni))
			return FALSE;
		if(directoryFni->FileNameLength/sizeof(WCHAR) != oldDirectoryNameLength)
			return FALSE;
		for(size_t i = 0; i < oldDirectoryNameLength; i++)
			if(directoryFni->FileName[i] != fileName[i])
				return FALSE;
	}

	return TRUE;
}


/* Thread Loops */

/**
 * Processes the saved directory events and calls the handler function.
 * 
 * @param lpParam the DirectoryEventListenerData
*/
static DWORD WINAPI DirectoryEventDispatcherLoop(LPVOID lpParam) {
	DirectoryEventListenerData *deld = (DirectoryEventListenerData*) lpParam;

	DirectoryEvent de;
	de.dirPath = deld->dirPath;

	FNI *fni = NULL;

	while(TRUE) {
		// Wait for events to be available
		_WaitForSingleObject(deld->availableEventsEvent, INFINITE);

		/*
		 * There are many directory change scenarios that report a chain events from a single action, the longest of which is 4 steps.
		 * In order to be able to analyse the action, the thread needs all relevant events already in the queue, therefore, if the queue
		 * has less than that number of elements, the thread allows a short time for those batches to reported and added to the queue.
		*/
		if(deld->eventQueue->length < 4)
			Sleep(DISPATCHER_LOOP_CHAIN_EVENTS_CHECK_SLEEP_TIME);

		fni = DequeueFNI(deld);

		switch(fni->Action) {
			case FILE_ACTION_ADDED:
				de.action = FILE_ACTION_ADDED;
				CopyFNIFileName(fni, &de.fileName);
				break;

			case FILE_ACTION_REMOVED:
				CopyFNIFileName(fni, &de.fileName);

				// Check if file was moved
				if(IsFileMoveAction(deld, de.fileName)) {
					de.action = FILE_ACTION_MOVED;
					de.oldFileName = de.fileName;
					Free(fni);
					fni = DequeueFNI(deld);
					CopyFNIFileName(fni, &de.fileName);
					Free(DequeueFNI(deld));
					break;
				}

				// Otherwise, it's a remove
				de.action = FILE_ACTION_REMOVED;
				break;

			case FILE_ACTION_MODIFIED:
				// If the modified file is a directory, ignore action
				if(FNIIsDirectory(deld->dirPath, fni))
					break;

				de.action = FILE_ACTION_MODIFIED;
				CopyFNIFileName(fni, &de.fileName);

				// If there are duplicate file modifications, skip them
				while(deld->eventQueue->length > 0 && FNICorrespondsTo(deld->eventQueue->head->data, FILE_ACTION_MODIFIED, de.fileName))
					Free(DequeueFNI(deld));
				break;

			case FILE_ACTION_RENAMED:
				// Broken rename event chain, ignore it
				if(deld->eventQueue->length == 0 || ((FNI*)(deld->eventQueue->head->data))->Action != FILE_ACTION_RENAMED_NEW_NAME)
					break;

				de.action = FILE_ACTION_RENAMED;
				CopyFNIFileName(fni, &de.oldFileName);
				Free(fni);
				fni = DequeueFNI(deld);
				CopyFNIFileName(fni, &de.fileName);
				break;

			// Unknown or unexpected action, ignore it
			default: break;
		}

		Free(fni);

		if(de.action != NULL_ACTION)
			deld->handlerFunction(de);

		de.action = NULL_ACTION;
		de.fileName = NULL;
		de.oldFileName = NULL;
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
  	overlapped.hEvent = _CreateEvent(NULL, FALSE, 0, NULL);

	FNI* currFni;

	while(TRUE) {
		// Wait for changes in the directory
		_ReadDirectoryChangesW(hDir, buffer, LISTENER_LOOP_BUFFER_SIZE, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL);
		_WaitForSingleObject(overlapped.hEvent, INFINITE);
		_GetOverlappedResult(hDir, &overlapped, &bytesTransferred, FALSE);

		// Save and enqueue the changes
		currFni = (FNI*) buffer;
		while(TRUE) {
			size_t fniSize = currFni->NextEntryOffset > 0 ? currFni->NextEntryOffset : bytesTransferred - ((BYTE*)currFni - buffer);
			FNI* fni = Malloc(fniSize);
			memcpy(fni, currFni, fniSize);
			EnqueueFNI(deld, fni);

			if(currFni->NextEntryOffset == 0)
				break;
			*((BYTE**)&currFni) += currFni->NextEntryOffset;
		};
	}

	return 0;
}


void StartDirectoryEventListener(WCHAR* dirPath, void (*handlerFunction) (DirectoryEvent de)) {
	DirectoryEventListenerData *deld = Malloc(sizeof(DirectoryEventListenerData));
	deld->dirPath = dirPath;
	deld->handlerFunction = handlerFunction;
	deld->eventQueue = CreateQueue();
	deld->queueMutex = _CreateMutex(NULL, FALSE, NULL);
	deld->availableEventsEvent = _CreateEvent(NULL, TRUE, FALSE, NULL);

	_CreateThread(NULL, 0, DirectoryEventListenerLoop, deld, 0, NULL);
	_CreateThread(NULL, 0, DirectoryEventDispatcherLoop, deld, 0, NULL);
}
