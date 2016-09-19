CC = g++
#DEBUG = -g

CFLAGS = -fPIC
source = kvs.c util.c
OBJS = kvs.o util.o
FPIC=-fPIC

dynamic : libbaseclient.so
static  : libbaseclient.a

.cpp.o:
	$(CC) -c $(CFLAGS) $(source)

libbaseclient.a : $(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)
	

libbaseclient.so : $(OBJS)
	rm -f $@
	$(CC) -shared -o $@ $(OBJS)

clean :
	rm -f *.o 
	rm -f libbaseclient.a
	rm -f libbaseclient.so
