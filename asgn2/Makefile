CC = gcc
CFLAGS = -Wall -g -std=gnu99

#all: 


 #run 'make rr_scheduler' to create rr_scheduler library
#rr_scheduler setup
rr_scheduler: rr_scheduler.a rr_scheduler.so
.PHONY: rr_scheduler #need this line to ignore main()/ executable

rr_scheduler.o: rr_scheduler.c lwp.h 
	$(CC) $(CFLAGS) -fPIC -I./include -o rr_scheduler.o -c rr_scheduler.c

#builds the rr_scheduler into a library
rr_scheduler.a: rr_scheduler.o
	ar rcs rr_scheduler.a rr_scheduler.o
	ranlib rr_scheduler.a

rr_scheduler.so: rr_scheduler.o
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ rr_scheduler.o

clean :
	rm -f *.o *.a *.so core.* 