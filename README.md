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
