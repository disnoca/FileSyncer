# File Syncer
A file syncer application that aims to keep directories of multiple clients syncronized between them by using a centralized server which also acts as a client.

### Payload
HEADER || CONTENT

HEADER: Magic Number || Message Type

CONTENT: Path Size || Path || Data Size || Data

Magic Number (4 bytes): 0x32C66E2F

Message Type (1 byte):
- 0x01: Create File
- 0x02: Delete File
- 0x00: Update File

### Configuration Files
- server.conf:
```
server hostname or ipv4 address
server port
```
- directorylinks.conf (server only):
```
serverdirectorylinkroot1     clientdirectorylinkroot1
serverdirectorylinkroot2     clientdirectorylinkroot2
serverdirectorylinkroot3     clientdirectorylinkroot3
[...]
```
