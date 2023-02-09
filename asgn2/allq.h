#include "lwp.h"
#include <stdbool.h>
bool a_onq(thread other);
void a_append(thread t);
thread a_pop(void);
thread a_first(void);
thread a_tid2thread(tid_t id);