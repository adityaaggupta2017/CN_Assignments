#include "client.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void *client_task(void *arg) {
  int sock = 0;
  struct sockaddr_in serv_addr;
  const char *request = "Get Top CPU-Consuming Processes";
  char buffer[BUFFER_SIZE] = {0};
  pthread_t this_id = pthread_self();

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Thread %lu: Socket creation error\n", this_id);
    return NULL;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "192.168.160.48", &serv_addr.sin_addr) <= 0) {
    printf("Thread %lu: Invalid address/Address not supported\n", this_id);
    close(sock);
    return NULL;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Thread %lu: Connection failed\n", this_id);
    close(sock);
    return NULL;
  }

  send(sock, request, strlen(request), 0);
  printf("Thread %lu: Request sent to server: %s\n", this_id, request);

  int valread = read(sock, buffer, BUFFER_SIZE);
  if (valread > 0) {
    printf("Thread %lu: Server reply: %s\n", this_id, buffer);
  } else {
    printf("Thread %lu: Failed to read response from server.\n", this_id);
  }

  close(sock);
  return NULL;
}

int main() {
  pthread_t t1, t2;

  if (pthread_create(&t1, NULL, client_task, NULL) != 0) {
    perror("Thread 1 creation failed");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&t2, NULL, client_task, NULL) != 0) {
    perror("Thread 2 creation failed");
    exit(EXIT_FAILURE);
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}
