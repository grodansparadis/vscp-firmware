#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>

void *cientThread(void *arg)
{  
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  printf("In client thread\n");

  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  //Configure settings of the server address
  // Address family is Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function 
  serverAddr.sin_port = htons(7799);

 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("localhost");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  //Connect the socket to the server using the address
  addr_size = sizeof serverAddr;
  if (connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) < 0) {
    printf("Connect failed\n");
    close(clientSocket);
    pthread_exit(NULL);
  }
  
  strcpy(message,"Hello my dear server\n");
  if (send(clientSocket, message ,strlen(message), 0/*MSG_DONTWAIT*/) < 0)
  {
    printf("Send failed\n");
    close(clientSocket);
    pthread_exit(NULL);
  }  

  // Read the message from the server into the buffer
  if (recv(clientSocket, buffer, 1024, 0) < 0) {
    printf("Receive failed\n");
    close(clientSocket);
    pthread_exit(NULL);
  }

  // Print the received message
  printf("Data received: %s\n",buffer);
  close(clientSocket);
  pthread_exit(NULL);
}

int main()
{
  int i = 0;
  int run = 0;
  pthread_t tid[51];

  printf("Client started\n");
  while(run < 50) {
    printf("Pthread %d started\n", run);
    if (pthread_create(&tid[run], NULL, cientThread, NULL) != 0) {
       printf("Failed to create thread\n");
    }
    printf("-------------------------\n");
    run++;
  }

  printf("Sleep\n");
  sleep(2);

  i = 0;
  while(i< 50) {
     pthread_join(tid[i++],NULL);
     printf("Run %d:\n",i);
  }
  return 0;
}
