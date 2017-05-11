#!/bin/sh


echo 'Build sample program:'

echo '---------------------------------'
echo '  Compiling...'
g++  -Wall -g -c -I../XMLClient -I../BaseClient xml.cpp -o xml.o

echo '  Linking...'
g++ -g -o xml xml.o -L../BaseClient -L../XMLClient -lbaseclient -lXMLClient -Wl,--no-as-needed -ldl
echo '  Done.'

