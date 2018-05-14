# copy-pasta

Distributed clipboard. Local applications can copy/pase data on a local
repository which is synchronized with other remote repositories possibly
 in different machines. 
 
Applications interact with the repositories through an API, which 
communicates via Unix domain sockets with the repository. The
repositories keep the data synched using internet domain sockets.
An instance of a repository can be started in single mode, in which case
 it is ready to accept connections from either applications or remote 
 repositories, or in connected mode, in which case it is bidirectionally 
 linked to another repository.


## Installing and using

Compile the clipboard with 
```
make clipboard
```
in the /src directory. Launch each local clipboard in a different 
directory. For connected mode, lauch with
```
./clipboard -c <ip_address> <port>
```
ip_address can be 127.0.0.1 for a local peer clipboard, and port is as
printed by the clipboard that we are connecting to.

The provided test application can be built with
```
make app_teste
```
again in the /src directory, and must be lauched in the same directory 
as the clipboard we are connecting to.
Alternatively, simply use
```
make 
```
in the copy-pasta directory to compile all provided files.

### Acknowledgments

* Darth Plagueis the wise
