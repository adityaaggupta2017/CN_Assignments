#include "server.h"

#define PORT 8080

void get_processes(Process process_stored[2]) {
  DIR *process_directory; // directory to store the information about the
                          // processes
  struct dirent *proc_entry;

  char starting_path[512];
  char buffer[1024] = {0};
  int pid;
  Process current_proc;

  memset(process_stored, 0,
         2 * sizeof(Process)); // Correct memory initialization

  process_directory = opendir("/proc"); // Open the /proc directory

  if (!process_directory) {
    perror("Failed to opendir!");
    return;
  }

  while (1) {
    proc_entry = readdir(process_directory);

    if (proc_entry == NULL) {
      break; // End of directory
    }

    if (isdigit(*proc_entry->d_name)) { // Only process directories with
                                        // numerical names (i.e., PID)
      pid = atoi(proc_entry->d_name);

      // Construct path of the form /proc/[pid]/stat
      snprintf(starting_path, sizeof(starting_path), "/proc/%d/stat", pid);

      int fd1 = open(starting_path, O_RDONLY); // Read the /proc/[pid]/stat file
      if (fd1 < 0) {
        continue;
      }

      read(fd1, buffer, sizeof(buffer)); // Store the result in the buffer
      close(fd1);

      // Parse process information from the stat file
      sscanf(buffer,
             "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
             &current_proc.pid, current_proc.name_proc, &current_proc.utime,
             &current_proc.stime);

      unsigned long total_time = current_proc.utime + current_proc.stime;

      if (total_time > process_stored[0].utime + process_stored[0].stime) {
        process_stored[1] = process_stored[0]; // Shift top process to second
        process_stored[0] = current_proc;      // Set new top process
      } else if (total_time >
                 process_stored[1].utime + process_stored[1].stime) {
        process_stored[1] = current_proc; // Set new second top process
      }
    }
  }

  closedir(process_directory);
}

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("Accept failed");
      exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr),
           ntohs(address.sin_port));

    // Receive the client's message
    int read_val = read(new_socket, buffer, 1024);
    if (read_val > 0) {
      printf("Received request from client: %s\n", buffer);
    }

    // Get top CPU-consuming processes
    Process process_stored[2];
    get_processes(process_stored);

    // Prepare the response with process details
    snprintf(buffer, sizeof(buffer),
             "Top 2 CPU-consuming processes are as follows:\n"
             "1. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n"
             "2. PID: %d, Name: %s, User Time: %lu, Kernel Time: %lu\n",
             process_stored[0].pid, process_stored[0].name_proc,
             process_stored[0].utime, process_stored[0].stime,
             process_stored[1].pid, process_stored[1].name_proc,
             process_stored[1].utime, process_stored[1].stime);

    // Send the process info to the client
    int sent_val = send(new_socket, buffer, strlen(buffer), 0);
    if (sent_val == -1) {
      perror("Failed to send response to client");
      exit(EXIT_FAILURE);
    }
    printf("Response sent to client with process details\n");

    close(new_socket);
  }

  close(server_fd);

  return 0;
}
