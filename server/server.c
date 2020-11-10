#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#define SIZE 1024
#define PORT 8080

void red () {
  printf("\033[0;31m");
}

void rederr () {
  fprintf(stderr, "\033[0;31m");
}

void green () {
  printf("\033[0;32m");
}

void yellow() {
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}

void reseterr () {
  fprintf(stderr,"\033[0m");
}


int send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};
  int val; char str[SIZE+1]={0}; 

  while((n=fread(data, sizeof(char), SIZE, fp)) != 0) {
  //  printf("%s\n",data);

    if ((send(sockfd, data, n,0)) == -1) {
      rederr();
      perror("Error in sending file.");
      reseterr();
      return 0;
    }
    val = recv(sockfd, str, SIZE+1, 0);


  //  bzero(str, SIZE+1);
    bzero(data, SIZE);
  }
  return 1;
}

int main(){
  int e, opt=1;long long fileSize;
  FILE *fp;
  char filename[SIZE]={0}; char str[SIZE]={0};int val;

  int sockfd, new_sock, valread;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE]={0};

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("socket error");
    exit(1);
  }
  // This is to lose the pesky "Address already in use" error message
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  
  printf("Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
  server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
  server_addr.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian


  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Binding successful.\n");
  

  if(listen(sockfd, 10) == 0){
    printf("Listening for client....\n");
  }
  else{
    perror("Error in listening");
    exit(EXIT_FAILURE);
  }

  addr_size = sizeof(new_addr);
  if((new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size))<0){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  yellow();
  printf("Connected to client\n");
  reset();
  while(1){
  valread = recv(new_sock , filename, 1024, 0);  // read infromation received into the buffer
  if(!strcmp(filename,""))
    break;
  printf("Requested file: \033[1;35m%s\033[0m\n",filename);
//  filename=buffer;

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    rederr();
    perror("Error in opening file");
    reseterr();
    send(new_sock,"-1",2,0);
    bzero(filename, SIZE);
  //  send(new_sock, strerror(errno), strlen(strerror(errno)), 0);
    continue;
  }
  fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp,0, SEEK_SET);
    char sizeString[100];
    sprintf(sizeString, "%lld", fileSize);
    send(new_sock, sizeString, strlen(sizeString), 0);
    val = recv(new_sock, str, SIZE, 0);
//  send(new_sock , "hello" , strlen("hello") , 0 );
  if(send_file(fp, new_sock)){
    green();
    printf("File data sent successfully.\n");
  }
  else{
    red();
    printf("Some error occurred, could not send file.\n");
  }
  reset();
  bzero(filename, SIZE);
  }
  yellow();
  printf("Connection closed\n");
  reset();
  return 0;
}