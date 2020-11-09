#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SIZE 1024
#define PORT 8080

void send_file(FILE *fp, int sockfd){
  int n;
  char data[SIZE] = {0};

  while(fread(data, sizeof(char), SIZE, fp) != 0) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

int write_file(int sockfd, char *filename, long long fileSize){
  int n;long long written=0;
  FILE *fp;
  char buffer[SIZE]={0};
  printf("writing %lld\n",fileSize);

  fp = fopen(filename, "w");
  while (1) {
    if(written>=fileSize) break;
    n = recv(sockfd, buffer, SIZE, 0);
  //  printf("%d\n", n);
    send(sockfd, "go", 2, 0);
    if (n <= 0){
      break;
    }
    written+=n;
  //  printf("%s", buffer);
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);

  }
//  printf("written=%lld\n", written);
  fclose(fp);
  return written;
}

int main(){
  char *ip = "127.0.0.1";
  int e,i,numCom=0;
    char buffer[1024] = {0};
    long long fileSize,b;
  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename;
  char tokens[200][1024];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("Error in socket");
    exit(1);
  }
  printf("Client socket created successfully.\n");

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

 while(1){
    char buffer[1024] = {0};
    char sz[100] = {0};
  printf("client> ");
  size_t inpSize = 0;
        char *input; char inp[1000];
        if(getline(&input,&inpSize,stdin)==-1){
          perror("input");
          break;
        }
    //    printf("%s\n", input);
        char* token = strtok(input, " \n");    

        numCom = 0;  
        while (token != NULL) { 
    //  command(token); 
    //  printf("%s\n",token );
          strcpy(tokens[numCom],token);
          ++numCom;
          token = strtok(NULL, " \n"); 
          
        } 
        if(numCom==1){
          if(!strcmp(tokens[0],"exit")){
            break;
          }
          else if(!strcmp(tokens[0],"get")){
            printf("Please provide name of file to be downloaded from server\n");
          }
          else{
            printf("command not found\n");
          }
        }
        else if(numCom>1){
          if(!strcmp(tokens[0],"get")){
            printf("%d file(s) to be downloaded\n", numCom-1);
            for(i=1;i<numCom;i++){
            //  bzero(buffer,SIZE);
            //  printf("hi\n");
              send(sockfd , tokens[i] , strlen(tokens[i]) , 0 );  // send the message.
              printf("%s download request sent\n", tokens[i]);
              int valread = recv( sockfd , buffer, 1024, 0);  // receive message back from server, into the buffer
            //  printf("%s\n", buffer);
              if(!strcmp(buffer,"-1")){
                printf("File not found.\n");
                continue;
              }
              send(sockfd,"got",3,0);
              printf("File found, starting download\n");
              fileSize = atoll(buffer);

              if((b=write_file(sockfd, tokens[i],fileSize))>=0){
                printf("Data %lld bytes written in the file successfully.\n",b);
              }
              else{
                printf("Error while downloading, please try again.\n\n");
              }
            }
          }
          else if(!strcmp(tokens[0],"exit")){
            break;
          }
          else{
            printf("command not found\n");
          }
        }
      }

//  int valread = read( sockfd , buffer, 1024);  // receive message back from server, into the buffer
//  printf("%s\n",buffer);
  


  printf("Closing the connection.\n");
  close(sockfd);

  return 0;
}