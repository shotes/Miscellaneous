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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "reqchannel.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
struct my_msgbuf {
	long mtype;
	char mtext[1024];
};

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

const bool VERBOSE = false;
int RequestChannel::count = 1;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/
RequestChannel::RequestChannel(const std::string _name, const Side _side) :
my_name(_name), my_side(_side), side_name((_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT")
{
    count += 1;
    msqid = -1;
    key_t key = ftok("a.txt",count);
    int perms = 0644;
    if(_side == SERVER_SIDE)
        perms |= IPC_CREAT;
    while(msqid < 0){
        msqid = msgget(key, perms);
    }
}

RequestChannel::~RequestChannel() {
    msgctl(msqid, IPC_RMID,NULL);
    //std::cout << "Deconstructed!" << std::endl;
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

std::string RequestChannel::send_request(std::string _request) {
	pthread_mutex_lock(&send_request_lock);
	if(cwrite(_request) < 0) {
		pthread_mutex_unlock(&send_request_lock);
		return "ERROR";
	}
	std::string s = cread();
	pthread_mutex_unlock(&send_request_lock);
	return s;
}

std::string RequestChannel::cread() {

    struct my_msgbuf buf;
    if(my_side == SERVER_SIDE){
        if(msgrcv(msqid,&buf, sizeof buf.mtext,2,0) <= 0){
            perror("msgrcv");
            exit(1);
        }
    }
    else{
        if(msgrcv(msqid,&buf, sizeof buf.mtext,1,0) <= 0){
            perror("msgrcv");
            exit(1);
        }

    }
	std::string s = buf.mtext;
	return s;
}

int RequestChannel::cwrite(std::string _msg) {

	if (_msg.length() >= MAX_MESSAGE) {
		std::cerr << my_name << ":" << side_name << "Message too long for Channel!" << std::endl;
		return -1;
	}

	struct my_msgbuf buf;

    int write_return_value;
    strcpy(buf.mtext,_msg.c_str());
    int len = strlen(buf.mtext);
    if(my_side == SERVER_SIDE){
        buf.mtype = 1;
        write_return_value = msgsnd(msqid, &buf, len+1, 0);
    }
    else{
        buf.mtype = 2;
        write_return_value = msgsnd(msqid, &buf, len+1, 0);
    }
	return write_return_value;
}

/*--------------------------------------------------------------------------*/
/* ACCESS THE NAME OF REQUEST CHANNEL  */
/*--------------------------------------------------------------------------*/

std::string RequestChannel::name() {
	return my_name;
}
