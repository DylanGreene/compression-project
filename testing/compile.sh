#!/bin/sh

g++ readImage.cpp -o readImage `pkg-config --cflags --libs opencv` -std=gnu++11

