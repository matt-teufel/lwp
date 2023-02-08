#include <stdbool.h>
#include <stdio.h>
#include "lwp.h"
#define STAILQ_NEXT(t)          ((t)->lib_one)
#include "stailq.h"

static STAILQ_HEAD(waiter) wq = STAILQ_INITIALIZER(wq);

static bool w_onq(thread other){
    STAILQ_FOREACH(thread, t, &wq){
            if(t == other){
                return true;
            }
    }
    return false;
}

void w_append(thread t){
    if(!w_onq(t)){
        STAILQ_INSERT_TAIL(&wq, t);
    }
    if(!w_onq(t)){
        perror("failed to add to wait queue");
    }
}

thread w_pop(void){
    thread first = STAILQ_FIRST(&wq);
    if(first){
        STAILQ_REMOVE_HEAD(&wq);
    }
    return first;
}