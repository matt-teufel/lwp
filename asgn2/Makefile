CC = gcc
CFLAGS = -Wall -g -std=gnu99

LD 	= gcc
LDFLAGS  = -Wall -g -L../lib64


SNAKEOBJS  = randomsnakes.o util.o
HUNGRYOBJS = hungrysnakes.o util.o
NUMOBJS    = numbersmain.o
OBJS	= $(SNAKEOBJS) $(HUNGRYOBJS) $(NUMOBJS) 

SNAKELIBS = -lPLN -lsnakes -lncurses -lrt

PROGS = lwp snakes nums hungry

all: lwp

magic64: magic64.S magic64.o

magic64.o: magic64.S
	$(CC) $(CFLAGS) -o magic64.o -c magic64.S

#lwp setup
#run 'make lwp' to create lwp library
lwp: liblwp.so
.PHONY: lwp #need this line to ignore main()/ executable

#builds lwp into a library
# lwp.a: lwp.o
# 	ar rcs lwp.a lwp.o
# 	ranlib lwp.a

#builds one library with scheduler.o, under lwp.so
liblwp.so: lwp.o rr_scheduler.o magic64.o	waitq.o	exitq.o
	$(CC) $(CFLAGS) -fPIC -I./include -shared -o $@ lwp.o rr_scheduler.o magic64.o	waitq.o	exitq.o

lwp.o: lwp.c lwp.h rr_scheduler.h magic64.o waitq.o	exitq.o
	$(CC) $(CFLAGS) -fPIC -I./include -o lwp.o -c lwp.c magic64.o waitq.o	exitq.o


#rr_scheduler setup
rr_scheduler: rr_scheduler.a rr_scheduler.so
.PHONY: rr_scheduler #need this line to ignore main()/ executable

rr_scheduler.o: rr_scheduler.c lwp.h rr_scheduler.h
	$(CC) $(CFLAGS) -fPIC -I./include -o rr_scheduler.o -c rr_scheduler.c

waitq:	waitq.a	waitq.so
.PHONY:	waitq

waitq.o:	waitq.c	lwp.h	waitq.h	stailq.h
	$(CC)	$(CFLAGS)	-fPIC	-I./include	-o	waitq.o	-c	waitq.c

exitq.o:	exitq.c	lwp.h	exitq.h	stailq.h
	$(CC)	$(CFLAGS)	-fPIC	-I./include	-o	exitq.o	-c	exitq.c

allq.o:	allq.c	lwp.h	allq.h	
	$(CC)	$(CFLAGS)	-fPIC	-I./include	-o	allq.o	-c	allq.c
# nums:	numbersmain.o	util.o	./liblwp.so
# 	$(LD)	$(LDFLAGS)	-o	nums	numbersmain.o	-llwp

#insert test files here
# snakes: randomsnakes.o util.o ../lib64/libPLN.so ../lib64/libsnakes.so
# 	$(LD) $(LDFLAGS) -o snakes randomsnakes.o util.o $(SNAKELIBS)

# hungry: hungrysnakes.o  util.o ../lib64/libPLN.so ../lib64/libsnakes.so
# 	$(LD) $(LDFLAGS) -o hungry hungrysnakes.o util.o $(SNAKELIBS)

# nums: numbersmain.o  util.o ../lib64/libPLN.so 
# 	$(LD) $(LDFLAGS) -o nums numbersmain.o -lPLN

# hungrysnakes.o: hungrysnakes.c ../include/lwp.h ../include/snakes.h
# 	$(CC) $(CFLAGS) -I ../include -c hungrysnakes.c

# randomsnakes.o: randomsnakes.c ../include/lwp.h ../include/snakes.h
# 	$(CC) $(CFLAGS) -I ../include -c randomsnakes.c

# numbermain.o: numbersmain.c lwp.h
# 	$(CC) $(CFLAGS) -I ../include -c numbersmain.c


# hungrysnakes.o: hungrysnakes.c ../include/lwp.h ../include/snakes.h
# 	$(CC) $(CFLAGS) -c hungrysnakes.c

# randomsnakes.o: randomsnakes.c ../include/lwp.h ../include/snakes.h
# 	$(CC) $(CFLAGS) -c randomsnakes.c

# numbermain.o: numbersmain.c lwp.h
# 	$(CC) $(CFLAGS) -c numbersmain.c

util.o: util.c lwp.h util.h snakes.h
	$(CC) $(CFLAGS) -c util.c


clean :
	rm -f *.o *.a *.so core.* 