

#ifndef _reqchannel_H_                   // include file only once
#define _reqchannel_H_

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
#include "KernelSemaphore.h"
//#include "SHMBoundedBuffer.h"

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

class RequestChannel {

public:

	typedef enum {SERVER_SIDE, CLIENT_SIDE} Side;

	typedef enum {READ_MODE, WRITE_MODE} Mode;

private:

	std::string   my_name = "";
	std::string side_name = "";
	Side     my_side;

	int shmid;
    char *dataserver, *dataclient;
    KernelSemaphore *full0;
    KernelSemaphore *full1;
    KernelSemaphore *empty0;
    KernelSemaphore *empty1;

	/*	Locks used to keep the dataserver from dropping requests.	*/
    //SHMBoundedBuffer *client_to_server, *server_to_client;
	pthread_mutex_t send_request_lock;

public:
    static int count;
	/* -- CONSTRUCTOR/DESTRUCTOR */

	RequestChannel(const std::string _name, const Side _side);
	/* Creates a "local copy" of the channel specified by the given name.
	 If the channel does not exist, the associated IPC mechanisms are
	 created. If the channel exists already, this object is associated with the channel.
	 The channel has two ends, which are conveniently called "SERVER_SIDE" and "CLIENT_SIDE".
	 If two processes connect through a channel, one has to connect on the server side
	 and the other on the client side. Otherwise the results are unpredictable.

	 NOTE: If the creation of the request channel fails (typically happens when too many
	 request channels are being created) and error message is displayed, and the program
	 unceremoniously exits.

	 NOTE: It is easy to open too many request channels in parallel. In most systems,
	 limits on the number of open files per process limit the number of established
	 request channels to 125.
	*/

	~RequestChannel();
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

	std::string name();
	/* Returns the name of the request channel. */
};


#endif


