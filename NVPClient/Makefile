CC = g++
#DEBUG = -g

CFLAGS = -DWITH_OPENSSL -DWITH_DOM -DLINUX -DCERTC_SW_PLATFORM=CERTC_SW_LINUX -DPTHREADS -DOPENSSL_SYS_UNIX -D_POSIX_THREADS
source = NVPClient.cpp soapC.cpp soapINVPTransactionProcessorProxy.cpp log.cpp safefields.cpp ../lib/gsoap-2.8/gsoap/stdsoap2.cpp ../lib/gsoap-2.8/gsoap/plugin/threads.c ../lib/gsoap-2.8/gsoap/dom.cpp ../lib/gsoap-2.8/gsoap/plugin/smdevp.c ../lib/gsoap-2.8/gsoap/plugin/mecevp.c ../lib/gsoap-2.8/gsoap/plugin/wsseapi.c
OBJS =  NVPClient.o soapC.o soapINVPTransactionProcessorProxy.o log.o safefields.o stdsoap2.o threads.o dom.o smdevp.o mecevp.o wsseapi.o
FPIC=-fPIC

EXTERNAL32 = ../lib/openssl-1.0.1t/linux32/lib
LIBDIRS32 = -L$(EXTERNAL32) -L$(EXTERNAL32) -L../BaseClient -L../lib/libxml2/linux32/lib
LIBS_DYNAMIC32 = -lssl -lcrypto -lbaseclient -lxml2
INCLUDES32 = -I../lib/gsoap-2.8/gsoap -I../lib/gsoap-2.8/gsoap/import -I../lib/gsoap-2.8/gsoap/plugin -I../lib/openssl-1.0.lt/linux32/include -I../BaseClient -I../lib/libxml2/linux32/include/libxml2

EXTERNAL64 = ../lib/openssl-1.0.1t/linux64/lib
LIBDIRS64 = -L$(EXTERNAL64) -L$(EXTERNAL64) -L../BaseClient -L../lib/libxml2/linux64/lib
LIBS_DYNAMIC64 = -lssl -lcrypto -lbaseclient -lxml2
INCLUDES64 = -I../lib/gsoap-2.8/gsoap -I../lib/gsoap-2.8/gsoap/import -I../lib/gsoap-2.8/gsoap/plugin -I../lib/openssl-1.0.1t/linux64/include -I../BaseClient -I../lib/libxml2/linux64/include/libxml2

32	: .cpp.o32 dynamic32 static

64	: .cpp.o64 dynamic64 static

dynamic32 : libNVPClient.so32
dynamic64 : libNVPClient.so64

static  : libNVPClient.a

.cpp.o32:
	$(CC) -c $(CFLAGS) -Wall $(FPIC) $(source) $(INCLUDES32) 

.cpp.o64:
	$(CC) -c $(CFLAGS) -Wall $(FPIC) $(source) $(INCLUDES64)



libNVPClient.a : $(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)
	

libNVPClient.so32 : $(OBJS)
	rm -f $@
	$(CC) -shared -o $@ $(OBJS) $(LIBDIRS32) $(LIBS_DYNAMIC32)
	mv libNVPClient.so32 libNVPClient.so

libNVPClient.so64 : $(OBJS)
	rm -f $@
	$(CC) -shared -o $@ $(OBJS) $(LIBDIRS64) $(LIBS_DYNAMIC64)
	mv libNVPClient.so64 libNVPClient.so

clean :
	rm -f *.o 
	rm -f libNVPClient.a
	rm -f libNVPClient.so
