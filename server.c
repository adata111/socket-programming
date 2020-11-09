#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#define SIZE 1024
#define PORT 8000

void send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};


  while(fgets(data, SIZE, fp) != NULL) {
  //  printf("%s\n",data);

    if ((send(sockfd, data, sizeof(data),0)) == -1) {

      perror("Error in sending file.");
      exit(1);
    }

    bzero(data, SIZE);
  }
}

int main(){
  int e, opt=1;
  FILE *fp;
  char filename[SIZE]={0};

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

  valread = read(new_sock , filename, 1024);  // read infromation received into the buffer
  printf("%s\n",filename);
//  filename=buffer;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Error in opening file.");
    send(new_sock, strerror(errno), strlen(strerror(errno)), 0);
    exit(1);
  }
//  send(new_sock , "hello" , strlen("hello") , 0 );
  send_file(fp, new_sock);
//  printf("File data sent successfully.\n");

  return 0;
}