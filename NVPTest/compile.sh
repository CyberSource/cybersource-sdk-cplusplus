#!/bin/sh


echo 'Build sample program:'

echo '---------------------------------'
echo '  Compiling...'
g++  -Wall -g -c -I../NVPClient -I../BaseClient NVPTest.cpp -o NVPTest.o

echo '  Linking...'
g++ -g -o NVPTest NVPTest.o -L../BaseClient -L../NVPClient -lbaseclient -lNVPClient -Wl,--no-as-needed -ldl
echo '  Done.'

