#!/bin/sh

execName=$(echo $(basename $1) | cut -d. -f1)
g++ $1 -o $execName `pkg-config --cflags --libs opencv` -std=gnu++11
