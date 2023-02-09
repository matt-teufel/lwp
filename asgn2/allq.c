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
    return NULL;
}

//update linked list to remove a thread that has been reaped 
void a_remove_thread(thread remove){
    if(STAILQ_FIRST(&aq) == remove){
        STAILQ_REMOVE_HEAD(&aq);
    }else{
        STAILQ_FOREACH(thread, t, &aq){
            if(t->lib_two == remove){
                t->lib_two = t->lib_two->lib_two; /* next becomes next next 
                                                    node in the middle will be deleted */
            }
        }
    }

}