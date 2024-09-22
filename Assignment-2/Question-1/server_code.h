#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// creating struct to store the information about the processes in the server
// and will send these to the client .

typedef struct {
  int pid;
  char name_proc[512];
  long unsigned int utime;
  long unsigned int stime;

} Process;