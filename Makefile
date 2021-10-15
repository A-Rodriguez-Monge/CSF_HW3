CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

csim: main.o Helper.o
	$(CC) $(CFLAGS) main.o Helper.o -o csim

main.o: main.cpp Helper.h
	$(CC) $(CFLAGS) -c main.cpp

Helper.o: Helper.cpp Helper.h
	$(CC) $(CFLAGS) -c Helper.cpp

clean:
	rm -f *.o main
	rm -f csim
	rm -f *.o Helper
