/*
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "NetworkRequestChannel.h"

const bool VERBOSE = false;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

NetworkRequestChannel::NetworkRequestChannel(const std::string _server_host_name, const std::string _port_no){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status;
    if ((status = getaddrinfo(_server_host_name.c_str(), _port_no.c_str(), &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(0);
    }

    // make a socket:
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0){
        perror ("Error creating socket\n");
        exit(0);
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
    {
        perror ("connect error\n");
        exit(0);
    }
    freeaddrinfo(res);
}

NetworkRequestChannel::NetworkRequestChannel(const std::string _port_no, void * (*connection_handler) (void *), int backlog){

    struct addrinfo hints, *serv;
    struct sockaddr_storage client_addr;
    int rv, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, _port_no.c_str(), &hints, &serv)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(0);
    }
    if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
        perror("server: socket");
        exit(0);
    }
    if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        exit(0);
    }
    freeaddrinfo(serv); // all done with this structure
    if (listen(sockfd, backlog) == -1) {
        perror("listen");
        exit(1);
    }
    char s[1024];
    while(1){
        socklen_t sin_size = sizeof client_addr;
        int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(client_addr.ss_family,&(((struct sockaddr_in*)&client_addr)->sin_addr),s, sizeof s);
        std::cout << "Accepted connection from " << s << std::endl;
        NetworkRequestChannel* n = new NetworkRequestChannel(new_fd);
        pthread_t tid;
        pthread_create(&tid,0,connection_handler,n);
    }
}

NetworkRequestChannel::NetworkRequestChannel(int sfd){
    sockfd = sfd;
}

NetworkRequestChannel::~NetworkRequestChannel() {
    close(sockfd);
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

std::string NetworkRequestChannel::send_request(std::string _request) {
    pthread_mutex_lock(&send_request_lock);
    if(cwrite(_request) < 0) {
        pthread_mutex_unlock(&send_request_lock);
        return "ERROR";
    }
    std::string s = cread();
    pthread_mutex_unlock(&send_request_lock);
    return s;
}

std::string NetworkRequestChannel::cread() {
    char buf[1024];
    if(recv(sockfd,buf,1024,0) == -1){
        perror("receive failed");
    }
    return buf;
}

int NetworkRequestChannel::cwrite(std::string _msg) {
    char buf[1024];
    strcpy(buf, _msg.c_str());
    if(send(sockfd,buf,strlen(buf)+1,0) == -1){
        perror("send failed");
    }
    return 0;
}

int NetworkRequestChannel::socket_fd(){
    return sockfd;
}
