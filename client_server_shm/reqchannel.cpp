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

#define SHM_SIZE 2048

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
#include <sys/ipc.h>
#include <sys/shm.h>

#include "reqchannel.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
struct my_msgbuf {
	long mtype;
	char mtext[SHM_SIZE/2];
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
    full0 = new KernelSemaphore(0,count++);
    empty0 = new KernelSemaphore(1,count++);
    full1 = new KernelSemaphore(0,count++);
    empty1 = new KernelSemaphore(1,count++);
    shmid = -1;
    key_t key = ftok("a.txt",count++);
    int perms = 0644;
    if(_side == SERVER_SIDE)
        perms |= IPC_CREAT;
    while(shmid < 0){
        shmid = shmget(key, SHM_SIZE, perms);
    }
    dataserver = (char*)shmat(shmid,0,0);
    dataclient = dataserver + SHM_SIZE/2;
}

RequestChannel::~RequestChannel() {
    shmdt(dataserver);
    delete full0;
    delete full1;
    delete empty0;
    delete empty1;
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
	std::string s;
    if(my_side == SERVER_SIDE){
        full1->P();
        s = dataserver;
        empty1->V();
    }
    else{
        full0->P();
        s = dataclient;
        empty0->V();
    }
	return s;
}

int RequestChannel::cwrite(std::string _msg) {
    if(my_side == SERVER_SIDE){
        empty0->P();
        strncpy(dataclient,_msg.c_str(),SHM_SIZE);
        full0->V();
    }
    else{
        empty1->P();
        strncpy(dataserver,_msg.c_str(),SHM_SIZE);
        full1->V();
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/* ACCESS THE NAME OF REQUEST CHANNEL  */
/*--------------------------------------------------------------------------*/

std::string RequestChannel::name() {
	return my_name;
}
