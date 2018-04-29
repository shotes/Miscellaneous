
#include <signal.h>
#include <iomanip>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include "NetworkRequestChannel.h"
#include "bounded_buffer.h"
#include <math.h>
#include <vector>
using namespace std;

volatile sig_atomic_t histogram_flag = false;

//Each request thread needs a pointer to the request buffer to send
struct PARAMS_REQUEST {
    std::string request_string;
	bounded_buffer* request_buffer;
    int num_requests;
	PARAMS_REQUEST(std::string s, bounded_buffer* bbp, int i)
		:request_string(s),request_buffer(bbp),num_requests(i){}
};
//Each worker thread needs a pointer to the request_return buffer and a pointer to the request buffer
//Each worker also needs a pointer to the control channel so that it can start each channel of its own
struct PARAMS_WORKER {
    NetworkRequestChannel* worker_channel;
    bounded_buffer* request_buffer;
    bounded_buffer* return_request_buffer_john;
    bounded_buffer* return_request_buffer_jane;
    bounded_buffer* return_request_buffer_joe;
    PARAMS_WORKER(NetworkRequestChannel* rcp,
                  bounded_buffer* rrbp_john,
                  bounded_buffer* rrbp_jane,
                  bounded_buffer* rrbp_joe,
                  bounded_buffer* rbp):
                  request_buffer(rbp),
                  return_request_buffer_john(rrbp_john),
                  return_request_buffer_jane(rrbp_jane),
                  return_request_buffer_joe(rrbp_joe),
                  worker_channel(rcp)
                  {}
};

struct PARAMS_EH {
    bounded_buffer* request_buffer;
    bounded_buffer* return_request_buffer_john;
    bounded_buffer* return_request_buffer_jane;
    bounded_buffer* return_request_buffer_joe;
    std::string h,p;
    int n, w;
    PARAMS_EH(int _n, int _w, std::string _h, std::string _p,
                  bounded_buffer* rrbp_john,
                  bounded_buffer* rrbp_jane,
                  bounded_buffer* rrbp_joe,
                  bounded_buffer* rbp):
                  n(_n), w(_w), h(_h), p(_p),
                  request_buffer(rbp),
                  return_request_buffer_john(rrbp_john),
                  return_request_buffer_jane(rrbp_jane),
                  return_request_buffer_joe(rrbp_joe)
                  {}
};
//Each stat thread needs a pointer to the return_request buffer and pointers to the stat vectors
struct PARAMS_STAT {
    std::string name;
    int num_responses;
    bounded_buffer* return_request_buffer;
    std::vector<int>* freq_count_vector;
	semaphore* freq_count_lock;
    PARAMS_STAT(std::string pname,
                int nr,
                bounded_buffer* rrbp,
                std::vector<int>* fcp,
				semaphore* fclp):
                name(pname),
                num_responses(nr),
                return_request_buffer(rrbp),
                freq_count_vector(fcp),
                freq_count_lock(fclp)
                {}
};

class atomic_standard_output {
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    ~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
    void print(std::string s){
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;

void print_time_diff(struct timeval * tp1, struct timeval * tp2, long* sec, long* musec) {
  /* Prints to stdout the difference, in seconds and museconds, between two
     timevals. */
  (*sec) = tp2->tv_sec - tp1->tv_sec;
  (*musec) = tp2->tv_usec - tp1->tv_usec;
  if ((*musec) < 0) {
    (*musec) += 1000000;
    (*sec)--;
  }
  printf(" [sec = %ld, musec = %ld] ", *sec, *musec);
}

std::string make_histogram(std::string name, std::vector<int> *data) {
    std::string results = "Frequency count for " + name + ":\n";
    for(int i = 0; i < data->size(); ++i) {
        results += std::to_string(i * 10) + "-" + std::to_string((i * 10) + 9) + ": " + std::to_string(data->at(i)) + "\n";
    }
    return results;
}

std::string make_histogram_table(std::string name1, std::string name2,
        std::string name3, std::vector<int> *data1, std::vector<int> *data2,
        std::vector<int> *data3) {
	std::stringstream tablebuilder;
	tablebuilder << std::setw(25) << std::right << name1;
	tablebuilder << std::setw(15) << std::right << name2;
	tablebuilder << std::setw(15) << std::right << name3 << std::endl;
	for (int i = 0; i < data1->size(); ++i) {
		tablebuilder << std::setw(10) << std::left
		        << std::string(
		                std::to_string(i * 10) + "-"
		                        + std::to_string((i * 10) + 9));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data1->at(i));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data2->at(i));
		tablebuilder << std::setw(15) << std::right
		        << std::to_string(data3->at(i)) << std::endl;
	}
	tablebuilder << std::setw(10) << std::left << "Total";
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data1->begin(), data1->end(), 0);
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data2->begin(), data2->end(), 0);
	tablebuilder << std::setw(15) << std::right
	        << accumulate(data3->begin(), data3->end(), 0) << std::endl;

	return tablebuilder.str();
}

//Each request thread needs a pointer to the request buffer to send
void* request_thread_function(void* arg) {
    PARAMS_REQUEST* args = (PARAMS_REQUEST*)arg;
    //PARAMS_REQUEST real_args = *args;
    int n = args->num_requests;
	for(int i = 0; i < n; i++) {
        // std::cout << args->request_string << std::endl;
		args->request_buffer->push_back(args->request_string);
	}
	return NULL;
}
//Each worker thread needs a pointer to the request_return buffer and a pointer to the request buffer
//Each worker also needs a pointer to the control channel so that it can start each channel of its own
void* worker_thread_function(void* arg) {
    PARAMS_WORKER* args = (PARAMS_WORKER*)arg;
     while(true) {
            //std::cout << "The size of the buffer is " << args->request_buffer->size() << std::endl;
            std::string request = args->request_buffer->retrieve_front();
            std::string response = args->worker_channel->send_request(request);
            if(request == "data John Smith") {
                args->return_request_buffer_john->push_back(response);
            }
            else if(request == "data Jane Smith") {
                args->return_request_buffer_jane->push_back(response);
            }
            else if(request == "data Joe Smith") {
                args->return_request_buffer_joe->push_back(response);
            }
            else if(request == "quit") {
                break;
            }
     }
     return NULL;
}


void* event_handler_thread_function(void* arg) {
    PARAMS_EH* args = (PARAMS_EH*)arg;

    int n = args->n;
    int w = args->w;
    std::string h = args->h;
    std::string p = args->p;
    bounded_buffer* rb = args->request_buffer;
    bounded_buffer* johnb = args->return_request_buffer_john;
    bounded_buffer* janeb = args->return_request_buffer_jane;
    bounded_buffer* joeb = args->return_request_buffer_joe;

    vector<string> state (w);
    int send_counter = 0, recv_counter = 0;
    NetworkRequestChannel** rc = new NetworkRequestChannel* [w];
    for (int i=0; i< w; i++)
    {
        rc [i] = new NetworkRequestChannel (h, p);
        state[i] = rb->retrieve_front();
        rc [i]->cwrite (state [i]);
        send_counter ++;
    }

    fd_set readset, backup;  // making the fd_set
    int fdmax;
    for (int i=0; i<w; i++)
    {
        fdmax = max (fdmax, rc[i]->socket_fd());
        FD_SET (rc [i]->socket_fd (), &readset);
    }
    backup = readset; // keep a backup

    while(recv_counter < 3*n) {
        readset = backup; //restore from backup because select() destroys the set
        int k = select (fdmax+1, &readset, 0,0,0);
        for (int i=0; i<w; i++){
            if (FD_ISSET(rc[i]->socket_fd(), &readset)){
                string resp = rc[i]->cread();
                recv_counter ++;
                string name = state [i].substr (5, 4);
                if (name == "John"){
                    johnb->push_back (resp);
                }
                else if (name == "Jane"){
                    janeb->push_back (resp);
                }
                else if (name == "Joe ")
                {
                    joeb->push_back (resp);
                }
                else{
                    assert (1 < 0);
                }

                if (send_counter < 3*n){
                    string more_req = rb->retrieve_front();
                    state [i] = more_req;
                    rc [i]->cwrite (more_req);
                    send_counter ++;
                }
            }//end if

        }// end for

    }//end while

    for (int i=0; i<w; i++)
    {
        rc [i]->send_request ("quit");
        delete rc [i];
    }
    return NULL;
}

//Each stat thread needs a pointer to the return_request buffer and pointers to the stat vectors
void* stat_thread_function(void* arg) {
    PARAMS_STAT* args = (PARAMS_STAT*)arg;
    int n = args->num_responses;
    for(int i = 0; i < n; i++){
        std::string response = args->return_request_buffer->retrieve_front();
        args->freq_count_lock->P();
        args->freq_count_vector->at(stoi(response) / 10) += 1;
        args->freq_count_lock->V();
    }
}
void handle_alarm( int sig ) {
    histogram_flag = true;
}
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {

    signal( SIGALRM, handle_alarm ); // Install handler first,
    alarm( 2 ); // before scheduling it to be called.

    long sec, musec;
	struct timeval start, stop;

    int n = 10; //default number of requests per "patient"
    int b = 50; //default size of request_buffer
    int w = 10; //default number of worker threads
    bool USE_ALTERNATE_FILE_OUTPUT = false;
    int opt = 0;
    std::string h = "127.0.0.1";
    std::string p = "9000";
    while ((opt = getopt(argc, argv, "n:b:w:h:p:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'h':
                h = optarg;
                break;
            case 'p':
                p = optarg;
                break;
            default:
                std::cout << "This program can be invoked with the following flags:" << std::endl;
                std::cout << "-n [int]: number of requests per patient" << std::endl;
                std::cout << "-b [int]: size of request buffer" << std::endl;
                std::cout << "-w [int]: number of worker threads" << std::endl;
                std::cout << "-p [int]: port number of server host" << std::endl;
                std::cout << "-h [string]: hostname" << std::endl;
                std::cout << "Example: ./client -n 10000 -b 50 -w 120 -p 9000 -h 127.0.0.1" << std::endl;
                std::cout << "If a given flag is not used, a default value will be given" << std::endl;
                std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
                std::cout << "behavior is the same as using the -h flag." << std::endl;
                exit(0);
        }
    }
    struct timeval start_time;
    struct timeval finish_time;
    int64_t start_usecs;
    int64_t finish_usecs;
    std::ofstream ofs;
    if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", std::ios::out | std::ios::app);
    else ofs.open("output.txt", std::ios::out | std::ios::app);

    std::cout << "n == " << n << std::endl;
    std::cout << "b == " << b << std::endl;
    std::cout << "w == " << w << std::endl;
    std::cout << "h == " << h << std::endl;
    std::cout << "p == " << p << std::endl;

    std::cout << "CLIENT STARTED" << std::endl;

    std::vector<int> john_frequency_count(10, 0);
    semaphore john_freq_count_mutex = semaphore(1);
    std::vector<int> jane_frequency_count(10, 0);
    semaphore jane_freq_count_mutex = semaphore(1);
    std::vector<int> joe_frequency_count(10, 0);
    semaphore joe_freq_count_mutex = semaphore(1);

    //TODO: Check if these buffers should be different sizes
    bounded_buffer request_buffer = bounded_buffer(b);
    bounded_buffer john_response_buffer = bounded_buffer(1000);
    bounded_buffer jane_response_buffer = bounded_buffer(1000);
    bounded_buffer joe_response_buffer = bounded_buffer(1000);

    PARAMS_REQUEST* john_requests_args = new PARAMS_REQUEST("data John Smith", &request_buffer, n);
    PARAMS_REQUEST* jane_requests_args = new PARAMS_REQUEST("data Jane Smith", &request_buffer, n);
    PARAMS_REQUEST* joe_requests_args = new PARAMS_REQUEST("data Joe Smith", &request_buffer, n);



    pthread_t john_request_thread;
    pthread_create(&john_request_thread, NULL, request_thread_function, john_requests_args);
    pthread_t jane_request_thread;
    pthread_create(&jane_request_thread, NULL, request_thread_function, jane_requests_args);
    pthread_t joe_request_thread;
    pthread_create(&joe_request_thread, NULL, request_thread_function, joe_requests_args);


    PARAMS_STAT* john_stat_args = new PARAMS_STAT("John",
                                                  n,
                                                  &john_response_buffer,
                                                  &john_frequency_count,
                                                  &john_freq_count_mutex);
    PARAMS_STAT* jane_stat_args = new PARAMS_STAT("Jane",
                                                  n,
                                                  &jane_response_buffer,
                                                  &jane_frequency_count,
                                                  &jane_freq_count_mutex);
    PARAMS_STAT* joe_stat_args = new PARAMS_STAT("Joe",
                                                 n,
                                                 &joe_response_buffer,
                                                 &joe_frequency_count,
                                                 &joe_freq_count_mutex);


    pthread_t john_response_thread;
    pthread_create(&john_response_thread, NULL, stat_thread_function, john_stat_args);
    pthread_t jane_response_thread;
    pthread_create(&jane_response_thread, NULL, stat_thread_function, jane_stat_args);
    pthread_t joe_response_thread;
    pthread_create(&joe_response_thread, NULL, stat_thread_function, joe_stat_args);

    gettimeofday(&start, NULL);

    pthread_t eh_thread;
    PARAMS_EH* eh_args = new PARAMS_EH(n , w, h, p,
                                                       &john_response_buffer,
                                                       &jane_response_buffer,
                                                       &joe_response_buffer,
                                                       &request_buffer);
    pthread_create(&eh_thread, NULL, event_handler_thread_function, eh_args);



    pthread_join(john_request_thread, NULL);
    pthread_join(jane_request_thread, NULL);
    pthread_join(joe_request_thread, NULL);

    pthread_join(eh_thread, NULL);

    pthread_join(john_response_thread, NULL);
    pthread_join(jane_response_thread, NULL);
    pthread_join(joe_response_thread, NULL);

    delete john_requests_args;
    delete jane_requests_args;
    delete joe_requests_args;
    delete john_stat_args;
    delete jane_stat_args;
    delete joe_stat_args;

    ofs.close();

    gettimeofday(&stop, NULL);
    std::cout << "--------------------------------------" << std::endl;
    print_time_diff(&start, &stop, &sec, &musec);
    std::cout << "\n--------------------------------------" << std::endl;

    std::string histogram_table = make_histogram_table("John Smith",
            "Jane Smith", "Joe Smith", &john_frequency_count,
            &jane_frequency_count, &joe_frequency_count);

    std::cout << histogram_table << std::endl;

    std::cout << "Sleeping..." << std::endl;
    usleep(10000);
}
