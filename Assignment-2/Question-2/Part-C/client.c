#include "client.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;

  char buffer[BUFFER_SIZE] = {0};

  char message[BUFFER_SIZE];

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

    perror("Socket creation error");

    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "192.168.160.48", &serv_addr.sin_addr) <= 0) {

    perror("Invalid address/ Address not supported");

    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection Failed!");

    exit(EXIT_FAILURE);
  }

  printf("Connection to server sent ! Enter a message : ");

  while (fgets(message, BUFFER_SIZE, stdin) != NULL) {

    message[strcspn(message, "\n")] = 0;

    send(sock, message, strlen(message), 0);

    printf("Message Sent\n");

    int valread = read(sock, buffer, sizeof(buffer));

    if (valread > 0) {

      buffer[valread] = '\0';

      printf("Server replied : %s\n", buffer);
    }

    printf("Enter a message: ");
  }

  close(sock);

  return 0;
}