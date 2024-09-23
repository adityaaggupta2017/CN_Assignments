// The client creates a socket and initiates the TCP connection. Your client
// process should support initiating “n” concurrent client connection requests,
// where “n” is passed as a program argument.

#include "client_code.h"
#include <complex.h>

#define PORT 8080

void *custom_handler(void *arg) {
  int socket_no = 0;

  struct sockaddr_in server_addr;

  char *message = "Get Top 2 CPU-Consuming processes!";

  char buffer[1024] = {0};

  socket_no = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_no < 0) {
    perror("Socket cant be created!\n");

    return NULL;
  }

  server_addr.sin_family = AF_INET;

  server_addr.sin_port = htons(PORT);

  // lets check for the servers address now ,
  // we are considering that the server is localhost for now .
  if (inet_pton(AF_INET, "10.0.2.15", &server_addr.sin_addr) <= 0) {
    printf("Invalid address / address does not work !\n");

    return NULL;
  }

  // now we can connect to the server
  
  int connect_val =
      connect(socket_no, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (connect_val < 0) {
    printf("Connection failed with server! \n");

    return NULL;
  }

  send(socket_no, message, strlen(message), 0);
  printf("Message sent from client\n");

  read(socket_no, buffer, 1024);

  printf("Server reply: %s\n", buffer);

  close(socket_no);

  return NULL;
}

int main(int argc, char const *argv[]) {

  if (argc != 2) {
    printf("Invalid Command !\n");

    return -1;
  }

  int num_of_clients = atoi(argv[1]);

  // now , we get the number of clients to be managed , lets create these many
  // number of threads for the same .

  pthread_t all_threads[num_of_clients];

  

  for (int i = 0; i < num_of_clients; i++) {
    int resp = pthread_create(&all_threads[i], NULL, custom_handler, NULL);
    if (resp != 0) {
      printf("Failed to created thread number : %d", (i + 1));

      return -1;
    }
  }

  for (int i = 0; i < num_of_clients; i++) {
    pthread_join(all_threads[i], NULL);
  }

}
