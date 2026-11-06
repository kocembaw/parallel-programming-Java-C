# Makefile dla zad_3 - program pthreads_zadanie3.c
# Uzycie:
#   make                       - kompiluje program
#   make run                   - uruchamia dla N = 4 watkow i przedzialu (0, 1)
#   make run N=8 A=-2 B=6      - uruchamia dla podanej liczby watkow i przedzialu
#   make clean                 - usuwa skompilowany program

# kompilator
CC = gcc
# opcje kompilacji: wszystkie ostrzezenia, informacje dla debuggera
CFLAGS = -Wall -g
# biblioteka watkow Pthreads
LDLIBS = -pthread
# nazwa programu (bez .c)
NAZWA = pthreads_zadanie3
# domyslne parametry uruchomienia (mozna nadpisac: make run N=8 A=-2 B=6)
N = 4
A = 0
B = 1

all: $(NAZWA)

$(NAZWA): $(NAZWA).c
	$(CC) $(CFLAGS) $(NAZWA).c -o $(NAZWA) $(LDLIBS)

run: $(NAZWA)
	./$(NAZWA) $(N) $(A) $(B)

clean:
	rm -f $(NAZWA)

.PHONY: all run clean
