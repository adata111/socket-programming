#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SIZE 1024
#define PORT 8000

void send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};

  while(fread(data, SIZE, 1, fp) != 0) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

void write_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "recv.txt";
  char buffer[SIZE]={0};

  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
    }
    printf("%s\n", buffer);
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}

int main(){
  char *ip = "127.0.0.1";
  int e;
    char buffer[1024] = {0};

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = "send.txt";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket1");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  memset(&server_addr, '0', sizeof(server_addr)); 

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("Connection error");
    return -1;
  }
 printf("Connected to Server.\n");

  send(sockfd , filename , strlen(filename) , 0 );  // send the message.
    printf("filename message sent\n");
//  int valread = read( sockfd , buffer, 1024);  // receive message back from server, into the buffer
//  printf("%s\n",buffer);
  write_file(sockfd);
  printf("Data written in the file successfully.\n");


  printf("Closing the connection.\n");
  close(sockfd);

  return 0;
}