all: GetEventCountsReduced GetEventCountsProd
CC=g++
CFLAGS= -g -std=c++11 -Wall `diana-config --cflags`
LDFLAGS= `diana-config --libs`

GetEventCountsReduced: GetEventCountsReduced.cpp
	$(CC) GetEventCountsReduced.cpp $(CFLAGS) $(LDFLAGS) -o GetEventCountsReduced
GetEventCountsProd: GetEventCountsProd.cpp
	$(CC) GetEventCountsProd.cpp $(CFLAGS) $(LDFLAGS) -o GetEventCountsProd

clean:
	rm ./GetEventCountsReduced ./GetEventCountsProd
