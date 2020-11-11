#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/stat.h>
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


int sendFile(FILE *fp, int sockfd){
  // this function returns 0 if complete file did not get sent properly and 1 otherwise
  int n;
  char data[SIZE] = {0};
  int val; char str[SIZE+1]={0}; 

  while((n=fread(data, sizeof(char), SIZE, fp)) != 0) { //read max 1024 bytes of data from file
  //  printf("%s\n",data);

    if ((send(sockfd, data, n,0)) == -1) {  // send all the data that has been read to client
      rederr();
      perror("Error in sending file.");
      reseterr();
      return 0;
    }
    val = recv(sockfd, str, SIZE+1, 0); //receive acknowlegement of receipt of the chunk that was sent
                                        //only after receiving acknowledgement the next chunk(if any) is sent

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
  
while(1){
  if(listen(sockfd, 10) == 0){   // 10 is the maximum size of queue - connections you haven't accepted
    printf("Listening for client....\n");
  }
  else{
    perror("Error in listening");
    exit(EXIT_FAILURE);
  }

  addr_size = sizeof(new_addr);
    // accept returns a new socket file descriptor to use for this single accepted connection.
  if((new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size))<0){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  yellow();
  printf("Connected to client\033[0m\n");
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
    send(new_sock,"-1",2,0);  // send negative fileSize to indicate the file is not available
                  // no need to wait for acknowledgement in this case as the next operation after continue is recv anyways
    bzero(filename, SIZE);
  //  send(new_sock, strerror(errno), strlen(strerror(errno)), 0);
    continue;
  }
  struct stat st;
  stat(filename, &st);
  if(S_ISREG(st.st_mode) <= 0){
    red();
    printf("File requested is not a regular file.\033[0m\n");
    reset();
    send(new_sock, "-2", 2, 0);
    bzero(filename, SIZE);
    continue;
  }
  fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);   //get the file size
    fseek(fp,0, SEEK_SET);  // set pointer to starting again
    char sizeString[100];
    sprintf(sizeString, "%lld", fileSize);    //convert file size to string
    send(new_sock, sizeString, strlen(sizeString), 0);  //send string file size to client
    val = recv(new_sock, str, SIZE, 0);  // wait to recieve acknowledgement of receipt of the file size
//  send(new_sock , "hello" , strlen("hello") , 0 );
  if(sendFile(fp, new_sock)){
    green();
    printf("File data sent successfully.\033[0m\n");
  }
  else{
    red();
    printf("Some error occurred, could not send file.\033[0m\n");
  }
  reset();
  bzero(filename, SIZE);
  }
  yellow();
  printf("Connection closed\033[0m\n");
  reset();
}
  return 0;
}