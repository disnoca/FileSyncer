# File Syncer
A file syncer application that aims to keep directories of multiple clients syncronized between them by using a centralized server which also acts as a client.

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
