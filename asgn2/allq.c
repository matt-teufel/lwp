#include <stdbool.h>
#include <stdio.h>
#include "lwp.h"
#define STAILQ_NEXT(t)          ((t)->lib_two)
#include "stailq.h"

static STAILQ_HEAD(all) aq = STAILQ_INITIALIZER(aq);

static bool a_onq(thread other){
    STAILQ_FOREACH(thread, t, &aq){
            if(t == other){
                return true;
            }
    }
    return false;
}

void a_append(thread t){
    if(!a_onq(t)){
        STAILQ_INSERT_TAIL(&aq, t);
    }
    if(!a_onq(t)){
        perror("failed to add to wait queue");
    }
}

thread a_pop(void){
    thread first = STAILQ_FIRST(&aq);
    if(first){
        STAILQ_REMOVE_HEAD(&aq);
    }
    return first;
}

thread a_first(void){
    return STAILQ_FIRST(&aq);
}

thread a_tid2thread(tid_t id){
    STAILQ_FOREACH(thread,t,&aq){
        if(t->tid==id){
            return t;
        }
    }
}