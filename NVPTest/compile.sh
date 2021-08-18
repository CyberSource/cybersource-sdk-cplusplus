#!/bin/sh


echo 'Build sample program:'

echo '---------------------------------'
echo '  Compiling...'
g++  -Wall -g -c -I../NVPClient -I../BaseClient NVPTest.cpp -o NVPTest.o

echo '  Linking...'
g++ -g -o NVPTest NVPTest.o -L../BaseClient -L../NVPClient -lbaseclient -lNVPClient -Wl,--no-as-needed -ldl -Wl,-rpath,'$ORIGIN/../lib/openssl-1.1.1k/linux64/lib' -L../lib/openssl-1.1.1k/linux64/lib -l:libssl.so.1.1 -l:libcrypto.so.1.1
echo '  Done.'

