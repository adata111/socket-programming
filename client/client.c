#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SIZE 1024
#define PORT 8080

void red () {
  printf("\033[0;31m");
}

void rederr () {
  fprintf(stderr,"\033[0;31m");
}

void green () {
  printf("\033[0;32m");
}

void yellow() {
  printf("\033[0;33m");
}

void reset () {
  printf("\033[0m");
}

void reseterr () {
  fprintf(stderr,"\033[0m");
}

int writeFile(int sockfd, char *filename, long long fileSize){
// This function returns the number of bytes written and -1 if some error occurs

  int n;long long written=0;
  float perc;
  FILE *fp;
  char buffer[SIZE]={0};
//  printf("writing %lld\n",fileSize);

  fp = fopen(filename, "wb");  // "wb" to write binary file, will be needed for non .txt files mostly
  while (1) {
    if(written>=fileSize) break;
    n = recv(sockfd, buffer, SIZE, 0);
  //  printf("%d\n", n);
    send(sockfd, "go", 2, 0);  // send acknowledgement that a chunk has been recieved
    if (n <= 0){
      if(n<0) written = -1;
      break;
    }
   // printf("%s", buffer);
    if((fwrite(buffer, sizeof(char), n, fp))<0){    //write to the new file
      written = -1;
      break;
    }
    written = written + n;    // increase count of bytes written
    perc = 100*((float)(written)/fileSize);
    printf("\r%.2f%% downloaded", perc);

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
    rederr();
    perror("Error in socket");
    reseterr();
    exit(1);
  }
  printf("Client socket created successfully.\n");

  memset(&server_addr, '0', sizeof(server_addr));  // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<=0)
    {
      red();
        printf("\nInvalid address/ Address not supported \n");
      reset();
        return -1;
    }

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    rederr();
    perror("Connection error");
    reseterr();
    return -1;
  }
  yellow();
 printf("Connected to Server.\n");
 reset();

 while(1){
    char buffer[1024] = {0};
    char sz[100] = {0};
  printf("\033[1;36mclient> \033[0m");
  size_t inpSize = 0;
        char *input; char inp[1000];
        if(getline(&input,&inpSize,stdin)==-1){
          rederr();
          perror("input");
          reseterr();
          break;
        }
    //    printf("%s\n", input);

        char* token = strtok(input, " \n\t");    

        numCom = 0;  
        //split input based on spaces, newlines and tabs
        while (token != NULL) { 
    //  command(token); 
    //  printf("%s\n",token );
          strcpy(tokens[numCom],token);
          ++numCom;
          token = strtok(NULL, " \n\t"); 
          
        } 
        if(numCom==1){
          if(!strcmp(tokens[0],"exit")){
            break;
          }
          else if(!strcmp(tokens[0],"get")){
            red();
            printf("Please provide name of file to be downloaded from server\n");
          }
          else{
            red();
            printf("Command not found\n");
          }
          reset();
        }
        else if(numCom>1){
          if(!strcmp(tokens[0],"get")){
            yellow();
            printf("%d file(s) to be downloaded\n", numCom-1);
            reset();
            for(i=1;i<numCom;i++){//send download requests one file at a time to server
              bzero(buffer,SIZE);
            //  printf("hi\n");
              send(sockfd , tokens[i] , strlen(tokens[i]) , 0 );  // send the name of file to be downloaded
              printf("\033[1;35m%s \033[1;37mdownload request sent\033[0m\n", tokens[i]);
              int valread = recv( sockfd , buffer, 1024, 0);  // receive message back from server, into the buffer
                          // this message contains size of file to be downloaded and "-1" if the file doesn't exist
            //  printf("%s\n", buffer);
              if(!strcmp(buffer,"-1")){
                printf("\033[0;31mFile not found.\n\033[0m");
                continue;
              }
              send(sockfd,"got",3,0);   //send acknowledgement of the receipt of file size
              green();
              printf("File found, starting download\n");
              reset();
              fileSize = atoll(buffer); // convert string file size to long long

              if((b=writeFile(sockfd, tokens[i],fileSize))>=0){
                green();
                printf("\nFile (%lld bytes) downloaded successfully.\n",b);
              }
              else{
                printf("\033[0;31m\nError while downloading, please try again.\n\033[0m");
              }
              reset();
            }
          }
          else if(!strcmp(tokens[0],"exit")){
            break;
          }
          else{
            red();
            printf("Command not found\n");
            reset();
          }
        }
      }

//  int valread = read( sockfd , buffer, 1024);  // receive message back from server, into the buffer
//  printf("%s\n",buffer);
  

yellow();
  printf("Closing the connection.\n");
  reset();
  close(sockfd);

  return 0;
}