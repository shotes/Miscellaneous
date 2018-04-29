
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "NetworkRequestChannel.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
pthread_mutex_t channel_mutex;
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel *_channel, const std::string & _request) {
    _channel->cwrite("hello to you too");
}

void process_data(NetworkRequestChannel *_channel, const std::string &  _request) {
    usleep(1000 + (rand() % 5000));
    _channel->cwrite(std::to_string(rand() % 100));
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(NetworkRequestChannel *_channel, const std::string & _request) {

    if (_request.compare(0, 5, "hello") == 0) {
        process_hello(_channel, _request);
    }
    else if (_request.compare(0, 4, "data") == 0) {
        process_data(_channel, _request);
    }
    else {
        _channel->cwrite("unknown request");
    }
}

void* conn_handler(NetworkRequestChannel *chan) {
    for(;;) {
        std::cout << std::flush;
        std::string request = chan->cread();
        if (request.compare("quit") == 0) {
            chan->cwrite("bye");
            usleep(10000);          // give the other end a bit of time.
            break;                  // break out of the loop;
        }
        process_request(chan, request);
    }
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
    std::string p = "9000";
    int b = 20;
    int opt = 0;
    while ((opt = getopt(argc, argv, "b:p:")) != -1) {
        switch (opt) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'p':
                p = optarg;
                break;
            default:
                std::cout << "This program can be invoked with the following flags:" << std::endl;
                std::cout << "-b [int]: backlog of the server socket" << std::endl;
                std::cout << "-p [string]: port number for data server" << std::endl;
                std::cout << "Example: ./client_solution" << std::endl;
                std::cout << "If a given flag is not used, a default value will be given" << std::endl;
                std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
                std::cout << "behavior is the same as using the -h flag." << std::endl;
                exit(0);
        }
    }
    std::cout << "b == " << b << std::endl;
    std::cout << "p == " << p << std::endl;
    std::cout << "SERVER STARTED" << std::endl;
	NetworkRequestChannel control_channel(p,(void* (*)(void*))conn_handler, b);
}

