#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>    // for open
#include <unistd.h>   // for close
#include <sys/types.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////////////////////
// socketThread
//

void  socketThread(int clientSocket)
{
  char client_message[2000];
  char buffer[1024];

   printf("In client\n");

  int newSocket = clientSocket;
  recv(newSocket , client_message , sizeof(client_message) , 0);

  // Send message to the client socket
  pthread_mutex_lock(&lock);
  char *message = malloc(sizeof(client_message) + 20);
  bzero(message, sizeof(message));
  printf("Greeting client \n");
  strcpy(message,"Server received : ");
  strcat(message, client_message);
  strcat(message,"\n");
  bzero(buffer, sizeof(buffer));
  strcpy(buffer, message);
  free(message);
  pthread_mutex_unlock(&lock);
  
  sleep(1);
  
  send(newSocket, buffer, strlen(buffer), 0);
  printf("Exit socketThread \n");
  shutdown(newSocket, SHUT_RDWR);
  // Can read data here to get everything.
  recv(newSocket, client_message, sizeof(client_message), MSG_DONTWAIT);
  close(newSocket);
  }
 

int main(){

  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  pid_t pid[50];

  //Create the socket.
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);

  // Configure settings of the server address struct
  // Address family = Internet
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function to use proper byte order
  serverAddr.sin_port = htons(7799); 

  //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //Set all bits of the padding field to 0
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  //Bind the address struct to the socket
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  //Listen on the socket, with 50 max connection requests queued
  if (listen(serverSocket,50) == 0 ) {
    printf("Listening\n");
  }
  else {
    printf("Error\n");
    exit(1);
  }

  pthread_t tid[60];
  int i = 0;
  while (1) {

    /*---- Accept call creates a new socket for the incoming connection ----*/
    addr_size = sizeof serverStorage;
    newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
    int pid_c = 0;

    if ((pid_c = fork())==0) {
      socketThread(newSocket);
    }
    else {
      pid[i++] = pid_c;
      if( i >= 49) {
        i = 0;
        while(i < 50) {
          waitpid(pid[i++], NULL, 0);
        }
        i = 0;
      }
    }
  }

  return 0;
}

