/*
    File: reqchannel.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#ifndef _NetworkRequestChannel_H_                   // include file only once
#define _NetworkRequestChannel_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <exception>
#include <string>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

class sync_lib_exception : public std::exception {
	std::string err = "failure in sync library";

public:
	sync_lib_exception() {}
	sync_lib_exception(std::string msg) : err(msg) {}
	virtual const char* what() const throw() {
		return err.c_str();
	}
};

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   R e q u e s t C h a n n e l */
/*--------------------------------------------------------------------------*/

class NetworkRequestChannel {
private:
    pthread_mutex_t send_request_lock;
    int sockfd;
public:

	/* -- CONSTRUCTOR/DESTRUCTOR */

	NetworkRequestChannel(const std::string _server_host_name, const std::string _port_no);
    //CLIENT CONSTRUCTOR

    NetworkRequestChannel(const std::string _port_no, void * (*connection_handler) (void *), int backlog);
    //SERVER CONSTRUCTOR
    NetworkRequestChannel(int sfd);

	~NetworkRequestChannel();
	/* Destructor of the local copy of the bus. By default, the Server Side deletes any IPC
	 mechanisms associated with the channel. */

	std::string send_request(std::string _request);
	/* Send a string over the channel and wait for a reply. */

	std::string cread();
	/* Blocking read of data from the channel. Returns a string of characters
	 read from the channel. Returns NULL if read failed. */

	int cwrite(std::string _msg);
	/* Write the data to the channel. The function returns the number of characters written
	 to the channel. */
    int socket_fd();
};


#endif


