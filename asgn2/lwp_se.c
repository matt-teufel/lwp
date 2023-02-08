#include "lwp.h"
#include "rr_scheduler.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>

void lwp_exit(int exitval);
tid_t lwp_create(lwpfun function, void* argument);
void lwp_start(void);
void lwp_yield(void);
tid_t lwp_gettitd(void);
scheduler lwp_get_scheduler(void);
void lwp_set_scheduler(scheduler sched);





int main(int argc, char *argv[]){
    lwp_start();
    lwp_exit(0);
    return 0;
}