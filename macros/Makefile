all: GetEventCounts
CC=g++
CFLAGS= -g -Wall `diana-config --cflags`
LDFLAGS= `diana-config --libs`

GetEventCounts: GetEventCounts.cpp
	$(CC) GetEventCounts.cpp $(CFLAGS) $(LDFLAGS) -o GetEventCounts.cpp

clean:
	rm ./GetEventCounts
