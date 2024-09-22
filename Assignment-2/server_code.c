#include "server_code.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080

// we will be following the steps mentioned in the TCP (as mentioned in the tut
// sheet)

void get_processes(Process process_stored[2]) {
  DIR *process_directory; // directory to store the information about the
                          // processes .
  struct dirent *proc_entry;

  char starting_path[512];

  char buffer[1024] = {0};

  int pid;

  Process current_proc;

  // lets give some memory to the process_stored , memory for storing the top 2
  // processes .

  memset(process_stored, 0, 2 * sizeof(process_stored));

  process_directory = opendir(
      "/proc"); // we have now stored the information in the process directory .

  // printf("This is the process directory: %p\n", (void *)process_directory);

  if (!process_directory){
    perror("Failed to opendir !") ; 
    return; 
  }


  // now we can iterate over each entry in the opendir and update to get the 2 most cpu-consuming processes .  

  while (1){

    proc_entry = readdir(process_directory) ; 
    
    if (proc_entry == NULL){
      break; // this will mark the end of the process directory . 
    }
    printf("This is the process name: %s\n" , proc_entry->d_name) ;
    
    if (isdigit(*proc_entry->d_name)){
      pid = atoi(proc_entry->d_name) ; 

      // now we got the pid , we need to construct the path of the form /proc/[pid]/stat ; 
      snprintf(starting_path, sizeof(starting_path), "/proc/%d/stat"   , pid) ; // stored the path in the variable starting_path defined above . 

      int fd1 = open(starting_path , O_RDONLY) ; // read only mode ; 

      if (fd1 < 0){
        continue; 
      }

      read(fd1 , buffer , sizeof(buffer)) ; // stored the result in the buffer which will be used to get the information . 

      close(fd1) ; 

      // printf("Buffer content: \n %s\n" , buffer) ; 

      sscanf(buffer, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*u %lu %lu", &current_proc.pid , current_proc.name_proc , &current_proc.utime , &current_proc.stime) ;  // thus all the info is now stored inside the current_proc variable , now we can use comparision and other operators to do the things easily . 

      unsigned long total_time = current_proc.utime + current_proc.stime ; 

      if (total_time > process_stored[0].utime + process_stored[0].stime){
        process_stored[1] = process_stored[0] ; //shifting the most to second_most ; 
        process_stored[0] = current_proc ; // changing the topmost process ; This way data of the second most is lost  . 

      }
      else if(total_time > process_stored[1].utime + process_stored[1].stime){
        process_stored[1] = current_proc ; // similiarly changing the secondmost top process . 
      }
       
    }
    else{

      printf("Not a digit pid!\n") ; 
    }
    
    printf("done!\n") ; 
  }

  printf("This is working!\n") ; 


  closedir(process_directory) ; 
  
}

void *custom_handler(void *socket_no) {
  int new_socket_no = *(int *)(socket_no);

  free(socket_no);

  char buffer[1024] = {0};

  Process process_stored[2] ; 


  // char *m1 = "Hello from server";

  int read_val = read(new_socket_no, buffer, 1024);

  if (read_val > 0) {
    printf("Recieved request from client: %s\n", buffer);
  }

  // now lets get the top two cpu-consuming processes for the client ; 

  get_processes(process_stored) ; 

  snprintf(buffer, sizeof(buffer), "Top 2 CPU consuming processes are as follows :\n 1. PID: %d , Name: %s , User Time: %lu , Kernel Time: %lu\n" "2. PID: %d , Name: %s , User Time: %lu , Kernel Time: %lu\n", 
  process_stored[0].pid , process_stored[0].name_proc , process_stored[0].utime , process_stored[0].stime , 
  process_stored[1].pid , process_stored[1].name_proc , process_stored[1].utime , process_stored[1].stime) ; 

  // now the whole result which needs to be send is stored in the buffer . Now , we can simply use the send command to send the data to the client . 

  send(new_socket_no, buffer, strlen(buffer), 0) ; 

  
  // close(new_socket_no);

  printf("Client terminated . \n");

  return 0;
}


int main() {

  int server_fd;

  int new_socket_created;

  int *new_socket_thread;

  struct sockaddr_in address;

  int len_of_addr = sizeof(address);
  // we first create a socket for the same in the server code . Now we can move
  // forward .
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == 0) {
    perror("Socket_failed");

    exit(EXIT_FAILURE);
  }

  // now we need to bind this socket to a specific IP address and port ;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");

    close(server_fd);

    exit(EXIT_FAILURE);
  }

  // now we need to listen the connections
  // taking the backlog as 10 ;
  int val_check = listen(server_fd, 10);

  if (val_check < 0) {
    perror("Listen Failed");

    close(server_fd);

    exit(EXIT_FAILURE);
  }

  printf("Server Listening on port number : %d\n ", PORT);

  // in the accept command we have to take the last argument as int*  .
  // new_socket_created =
  //     accept(server_fd, (struct sockaddr *)&address, (socklen_t
  //     *)&len_of_addr);

  // if (new_socket_created < 0) {
  //   perror("Accept Failed");
  //   close(server_fd);

  //   exit(EXIT_FAILURE);
  // }

  // printf("Connection Accepted Successfully\n");

  // close(new_socket_created);

  // now we saw the above commented code for a single client handling , we need
  // to handle more than 1 client for this assignment

  // we can start a while loop in that condition .
  while (1) {
    int new_socket_created = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&len_of_addr);

    if (new_socket_created < 0) {
      perror("Accept failed for a thread!");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

    // now lets print the name of the client socket
    // inet_ntoa - internet network address to ascii , its converts the client
    // IP address from a binary format which is in the sin_addr to readable
    // string . ntohs - "Network to host short" -> readable format for host
    // machine .
    printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr),
           ntohs(address.sin_port));

    // we can create thread for the same  .
    pthread_t new_thread_id;

    new_socket_thread = malloc(sizeof(int));
    *new_socket_thread =
        new_socket_created; // storing the contents in the new_socket_thread ;

    int response_thread = pthread_create(&new_thread_id, NULL, custom_handler,
                                         (void *)new_socket_thread);

    if (response_thread < 0) {
      perror("Thread creation failed !");
      close(new_socket_created);
      free(new_socket_thread);
      exit(EXIT_FAILURE);
    }

    printf("Handler is assigned for the client task!\n");

    // we can free the memory after the work
    pthread_detach(new_thread_id);
  }

  close(server_fd);

  return 0;
}