#ifndef RR_SCHEDULERH
#define RR_SCHEDULERH
#include "lwp.h"
#endif

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next();
