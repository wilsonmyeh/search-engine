CC=g++
# I tend to use nullptr and other C++11 features
# It's good to get warnings
CFLAGS=-std=c++11 -Wall
ODIR=bin

all: msort

msort: msort.cpp
	$(CC) -o $(ODIR)/$@ $(CFLAGS) $^

.PHONY: clean
clean:
	rm -f $(ODIR)/*
