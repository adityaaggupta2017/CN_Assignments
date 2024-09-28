#include "client.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  const char *request = "Get Top CPU-Consuming Processes";
  char buffer[BUFFER_SIZE] = {0};

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation error");
    return -1;
  }

  // Server address setup
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IP address from text to binary form
  if (inet_pton(AF_INET, "192.168.160.48", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address/Address not supported");
    return -1;
  }

  // Connect to the server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    return -1;
  }

  // Send request for top CPU-consuming processes
  send(sock, request, strlen(request), 0);
  printf("Request sent to server: %s\n", request);

  // Receive the response from the server
  int valread = read(sock, buffer, BUFFER_SIZE);
  if (valread > 0) {
    printf("Server reply: %s\n", buffer);
  } else {
    printf("Failed to read response from server.\n");
  }

  // Close the socket
  close(sock);

  return 0;
}
