CC = gcc
CFLAGS = -Wall -g -std=gnu99 -I../include

LD 	= gcc
LDFLAGS  = -Wall -g -L../liblwp64 


SNAKEOBJS  = randomsnakes.o util.o
HUNGRYOBJS = hungrysnakes.o util.o
NUMOBJS    = numbersmain.o
OBJS	= $(SNAKEOBJS) $(HUNGRYOBJS) $(NUMOBJS) 

SNAKELIBS = -llwp -lsnakes -lncurses -lrt

PROGS = lwp mv snakes nums hungry

all: $(PROGS)

magic64: magic64.S magic64.o
magic64.o: magic64.S
	$(CC) $(CFLAGS) -o magic64.o -c magic64.S

#lwp setup
lwp: liblwp.so
.PHONY: lwp #need this line to ignore main()/ executable

#builds one library with scheduler.o, under lwp.so
liblwp.so: lwp.o rr_scheduler.o magic64.o waitq.o exitq.o allq.o
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ lwp.o rr_scheduler.o magic64.o	waitq.o exitq.o allq.o

lwp.o: lwp.c lwp.h rr_scheduler.h magic64.o waitq.o exitq.o allq.o
	$(CC) $(CFLAGS) -fPIC -I./include -o lwp.o magic64.o waitq.o exitq.o allq.o -c lwp.c


#rr_scheduler setup
rr_scheduler: rr_scheduler.o
.PHONY: rr_scheduler #need this line to ignore main()/ executable

rr_scheduler.o: rr_scheduler.c lwp.h rr_scheduler.h
	$(CC) $(CFLAGS) -fPIC -I./include -o rr_scheduler.o -c rr_scheduler.c

#queue setups converted into a library
waitq:  waitq.so
exitq:  exitq.so
allq:   allq.so
.PHONY:	waitq exitq allq

waitq.so: waitq.o 
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ waitq.o
waitq.o: waitq.c lwp.h   waitq.h stailq.h
	$(CC) $(CFLAGS)   -fPIC   -I./include -o  waitq.o -c  waitq.c

exitq.so: exitq.o 
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ exitq.o
exitq.o: exitq.c lwp.h   exitq.h stailq.h
	$(CC) $(CFLAGS)   -fPIC   -I./include -o  exitq.o -c  exitq.c

allq.so: allq.o 
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ allq.o
allq.o: allq.c  lwp.h   allq.h
	$(CC) $(CFLAGS)   -fPIC   -I./include -o  allq.o  -c  allq.c


#insert test files here
.PHONY: lwps
lwps: lwp_se.o liblwp.so
	$(CC) $(CFLAGS) -L. -llwp lwp_se.o -o lwps

lwp_se.o: lwp_se.c lwp.h rr_scheduler.h
	$(CC) $(CFLAGS) -c lwp_se.c

lwp_run: lwps
		LD_LIBRARY_PATH=. ./lwps  


snakes: randomsnakes.o util.o ../liblwp64/liblwp.so ../lib64/libsnakes.so
	$(LD) $(LDFLAGS) -o snakes randomsnakes.o util.o $(SNAKELIBS)

hungry: hungrysnakes.o  util.o ../liblwp64/liblwp.so ../lib64/libsnakes.so
	$(LD) $(LDFLAGS) -o hungry hungrysnakes.o util.o $(SNAKELIBS)

nums: numbersmain.o  util.o ../liblwp64/liblwp.so 
	$(LD) $(LDFLAGS) -o nums numbersmain.o -llwp

hungrysnakes.o: hungrysnakes.c ../include/lwp.h ../include/snakes.h
	$(CC) $(CFLAGS) -c hungrysnakes.c 

randomsnakes.o: randomsnakes.c ../include/lwp.h ../include/snakes.h
	$(CC) $(CFLAGS) -c randomsnakes.c 

numbermain.o: numbersmain.c lwp.h
	$(CC) $(CFLAGS) -c numbersmain.c

util.o: util.c ../include/lwp.h ../include/util.h ../include/snakes.h
	$(CC) $(CFLAGS) -c util.c 

rs: snakes
	(export LD_LIBRARY_PATH=../liblwp64; ./snakes)

hs: hungry
	(export LD_LIBRARY_PATH=../liblwp64; ./hungry)

ns: nums
	(export LD_LIBRARY_PATH=../liblwp64; ./nums)

mv: lwp
	(mv liblwp.so ../liblwp64)

clean :
	rm -f lib64 *.o *.a *.so core.* 