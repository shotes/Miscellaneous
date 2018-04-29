//
//  bounded_buffer.hpp
//  
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#ifndef bounded_buffer_h
#define bounded_buffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include "semaphore.h"

class bounded_buffer {
	/* Internal data here */
	semaphore queue_lock_mutex = NULL;
	semaphore full = NULL;
	semaphore empty = NULL;
	std::queue<std::string> requests;
	int max_buffer_size;

public:
    bounded_buffer(int _capacity);
    ~bounded_buffer();
	std::queue<std::string> getQueue(){
		return requests;
	}
    void push_back(std::string str);
    std::string retrieve_front();
    int size();
};

#endif /* bounded_buffer_h */
