#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include "http_server.hh"

using namespace std;



void error(char *msg) {
  perror(msg);
  exit(1);
}
int numclient=0;
int clean=0;
pthread_cond_t c1=PTHREAD_COND_INITIALIZER;
pthread_cond_t c2=PTHREAD_COND_INITIALIZER;



pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;


//for thread processing
void *start_function(void *arg) {
  string temp;
    char buffer[1024];char response_buffer[1024];
    while(1) {
           pthread_mutex_lock(&m);
           while(!numclient) {
            pthread_cond_wait(&c1,&m);

           }
           if (numclient<0){
           pthread_mutex_unlock(&m);
            break;
    }
           int newsockfd = (*((queue<int> *) arg)).front();
           (*((queue<int> *) arg)).pop();
    numclient--;
    pthread_cond_signal(&c2);
    //cout << "worker working with" <<  " " << no_clients << '\n';
    pthread_mutex_unlock(&m);
    
   
     //receive input from client

      bzero(buffer, 1024);
       int n = read(newsockfd, buffer, 1023);

      if(n<0) {
        cerr<<"Error in listening to socket\n";
      }
      cout<<"The HTTP request you entered is:"<<'\n'<<buffer;
      //string FromClient(buffer);
      HTTP_Response *FromServer=handle_request(buffer);
      temp = FromServer->get_string();
      delete FromServer;
      strcpy(response_buffer,temp.c_str());
       n=write(newsockfd,response_buffer, strlen(response_buffer));
      if(n<0) {
        cerr<<"Error in writing"<<'\n';
      }
    
    close(newsockfd);
}
pthread_exit(NULL);
return 0;

}
void sighandler(int sig) {
  clean=1;
}

int main(int argc,char *argv[]) {
 int sockfd, newsockfd, portno;
  socklen_t clilen;

  //char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  queue<int> clients_fd;
  pthread_t threads[512];

  if(argc<2) {
    cerr<<"ERROR, no port provided\n";
    exit(1);
  }

  //create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket"); }

   /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    error("ERROR on binding"); }

  
  for(int i=0;i<512;i++) {
    if(pthread_create(&threads[i], NULL, start_function, (void*)&clients_fd)<0) {
      cerr<<"Couldn't create p_thread\n";
      exit(1);
    }


  }

  while(1) {

    while(numclient==512){
          pthread_cond_wait(&c2,&m);
      }
    /* listen for incoming connection requests */
    cout<<"Send Requests"<<'\n';
    listen(sockfd, 10000);
    clilen = sizeof(cli_addr);

    /* accept a new request, create a newsockfd */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if(newsockfd<0) {
      cerr<<"ERROR on accept\n";
    }

    pthread_mutex_lock(&m);
    // clients_fd.push(newsockfd);
    clients_fd.push(newsockfd);
    //counter++;
    numclient++;
    cout<<"New Thread in Queue with fd"<<newsockfd<<'\n';
    pthread_cond_signal(&c1); 
    pthread_mutex_unlock(&m);
  }

  numclient--;

  // for(int i = 0; i <511; i++)  {
  //   pthread_cond_signal(&c1);
  // } 
  // for(int i=0;i<512;i++) {
  //   pthread_kill(threads[i],SIGKILL);
  // }
  // for(int i = 0; i < 512; i++) {
    
  //   pthread_join(threads[i], NULL);
  // }
  // pthread_exit(NULL);


return 0;


}