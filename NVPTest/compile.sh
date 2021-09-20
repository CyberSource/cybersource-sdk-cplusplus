#!/bin/sh

if [ -z $1 ]; then
   echo "Use compiler.sh 32 or compiler.sh 64 to compile for 32 or 64 bit"
   exit 1
fi

tVersion=$1

echo 'Build sample program:'

echo '---------------------------------'
echo '  Compiling...'
g++  -Wall -g -c -I../NVPClient -I../BaseClient NVPTest.cpp -o NVPTest.o

echo '  Linking...'
g++ -g -o NVPTest NVPTest.o -L../BaseClient -L../NVPClient -lbaseclient -lNVPClient -Wl,--no-as-needed -ldl -Wl,-rpath,'$ORIGIN/../lib/openssl-1.1.1l/linux'${tVersion}'/lib' -L../lib/openssl-1.1.1l/linux${tVersion}/lib -l:libssl.so.1.1 -l:libcrypto.so.1.1
echo '  Done.'

