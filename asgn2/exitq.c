#include <stdbool.h>
#include <stdio.h>
#include "lwp.h"
#define STAILQ_NEXT(t)          ((t)->exited)
#include "stailq.h"

static STAILQ_HEAD(exiter) eq = STAILQ_INITIALIZER(eq);

static bool e_onq(thread other){
    STAILQ_FOREACH(thread, t, &eq){
            if(t == other){
                return true;
            }
    }
    return false;
}

void e_append(thread t){
    if(!e_onq(t)){
        STAILQ_INSERT_TAIL(&eq, t);
    }
    if(!e_onq(t)){
        perror("failed to add to exit queue");
    }
}

thread e_pop(void){
    thread first = STAILQ_FIRST(&eq);
    if(first){
        STAILQ_REMOVE_HEAD(&eq);
    }
    return first;
}