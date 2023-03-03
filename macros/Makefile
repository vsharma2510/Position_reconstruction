all: GetEventCountsReduced
CC=g++
CFLAGS= -g -Wall `diana-config --cflags`
LDFLAGS= `diana-config --libs`

GetEventCountsReduced: GetEventCountsReduced.cpp
	$(CC) GetEventCountsReduced.cpp $(CFLAGS) $(LDFLAGS) -o GetEventCountsReduced

clean:
	rm ./GetEventCountsReduced
