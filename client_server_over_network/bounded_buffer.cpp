
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#include "bounded_buffer.h"
#include <string>
#include <queue>

bounded_buffer::bounded_buffer(int _capacity) {
    requests = std::queue<std::string>();
	queue_lock_mutex = semaphore(1);
	empty = semaphore(_capacity);
	full = semaphore(0);
}

bounded_buffer::~bounded_buffer(){
}

void bounded_buffer::push_back(std::string req) {
    empty.P();
	queue_lock_mutex.P();
	requests.push(req);
	queue_lock_mutex.V();
	full.V();
}

std::string bounded_buffer::retrieve_front() {
    full.P();
	queue_lock_mutex.P();
	std::string s = requests.front();
	requests.pop();
	queue_lock_mutex.V();
	empty.V();
	return s;
}

int bounded_buffer::size() {
	queue_lock_mutex.P();
	int n = requests.size();
	queue_lock_mutex.V();
    return n;
}
