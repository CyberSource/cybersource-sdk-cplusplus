#!/bin/sh


echo 'Build sample program:'

echo '---------------------------------'
echo '  Compiling...'
g++  -Wall -g -c -I../XMLClient -I../BaseClient xml.cpp -o xml.o

echo '  Linking...'
g++ -g -o xml xml.o -L../BaseClient -L../XMLClient -lbaseclient -lXMLClient -Wl,--no-as-needed -ldl -Wl,-rpath,'$ORIGIN/../lib/openssl-1.1.1k/linux64/lib' -L../lib/openssl-1.1.1k/linux64/lib -l:libssl.so.1.1 -l:libcrypto.so.1.1
echo '  Done.'

