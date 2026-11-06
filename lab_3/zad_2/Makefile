# Makefile dla zad_2 - program pthreads_zadanie2.c
# Uzycie:
#   make                 - kompiluje program
#   make run             - uruchamia oba warianty dla N = 8
#   make run N=16        - uruchamia oba warianty dla podanej liczby watkow
#   make iterator N=16   - tylko wariant z iteratorem petli (bledny)
#   make tablica N=16    - tylko wariant z dedykowana tablica (poprawny)
#   make clean           - usuwa skompilowany program

# kompilator
CC = gcc
# opcje kompilacji: wszystkie ostrzezenia, informacje dla debuggera
CFLAGS = -Wall -g
# biblioteka watkow Pthreads
LDLIBS = -pthread
# nazwa programu (bez .c)
NAZWA = pthreads_zadanie2
# domyslna liczba watkow (mozna nadpisac: make run N=20)
N = 8

all: $(NAZWA)

$(NAZWA): $(NAZWA).c
	$(CC) $(CFLAGS) $(NAZWA).c -o $(NAZWA) $(LDLIBS)

run: $(NAZWA)
	./$(NAZWA) $(N)

iterator: $(NAZWA)
	./$(NAZWA) $(N) iterator

tablica: $(NAZWA)
	./$(NAZWA) $(N) tablica

clean:
	rm -f $(NAZWA)

.PHONY: all run iterator tablica clean
