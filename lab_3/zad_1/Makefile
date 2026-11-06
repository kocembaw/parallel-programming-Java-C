# Makefile dla zad_1 - program pthreads_detach_kill.c
# Uzycie:
#   make        - kompiluje program
#   make run    - kompiluje (jesli trzeba) i uruchamia program
#   make clean  - usuwa skompilowany program

# --- zmienne ---
# (komentarz zawsze w osobnej linii - inaczej spacje przed # trafia do wartosci zmiennej)

# kompilator
CC = gcc
# opcje kompilacji: wszystkie ostrzezenia, informacje dla debuggera
CFLAGS = -Wall -g
# biblioteka watkow Pthreads
LDLIBS = -pthread
# nazwa programu (bez .c)
NAZWA = pthreads_detach_kill

# --- reguly ---

# pierwsza regula jest domyslna - wykonuje ja samo "make"
all: $(NAZWA)

# jak zbudowac program: potrzebny jest plik .c; jesli .c jest nowszy niz program, kompiluj ponownie
$(NAZWA): $(NAZWA).c
	$(CC) $(CFLAGS) $(NAZWA).c -o $(NAZWA) $(LDLIBS)

# uruchomienie programu (najpierw upewnia sie, ze jest skompilowany)
run: $(NAZWA)
	./$(NAZWA)

# sprzatanie - usuniecie skompilowanego programu
clean:
	rm -f $(NAZWA)

# all, run i clean to nazwy polecen, a nie plikow
.PHONY: all run clean
