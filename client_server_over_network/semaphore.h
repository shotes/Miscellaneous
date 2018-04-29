
#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   s e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class semaphore {
private:
    /* -- INTERNAL DATA STRUCTURES */
    pthread_mutex_t counter_lock;
    pthread_cond_t wait_queue;
    int counter;
public:

    /* -- CONSTRUCTOR/DESTRUCTOR */

    semaphore(int initial_counter) {
        pthread_mutex_init(&counter_lock, NULL);
        pthread_cond_init(&wait_queue, NULL);
        counter = initial_counter;
	}

    ~semaphore(){
        pthread_mutex_destroy(&counter_lock);
        pthread_cond_destroy(&wait_queue);
    }

    /* -- SEMAPHORE OPERATIONS */
    
    void P() {
        pthread_mutex_lock(&counter_lock);
        --counter;
        if(counter < 0){
            pthread_cond_wait(&wait_queue, &counter_lock);
        }
        pthread_mutex_unlock(&counter_lock);
    }

    void V() {
        pthread_mutex_lock(&counter_lock);
        ++counter;
        if(counter <= 0){ 
            pthread_cond_signal(&wait_queue);
        }
        pthread_mutex_unlock(&counter_lock);
    }
};

#endif


