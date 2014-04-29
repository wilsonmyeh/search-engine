CC=g++-4.8
CFLAGS=-std=c++11 -Wall
ODIR=bin

all: webcrawler

webcrawler: webcrawler.cpp
	$(CC) -o $(ODIR)/$@ $(CFLAGS) $^

.PHONY: clean
clean:
	rm -f $(ODIR)/*
