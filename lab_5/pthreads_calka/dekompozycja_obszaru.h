#ifndef _dekompozycja_obszaru_
#define _dekompozycja_obszaru_

// struktura z zadaniem dla jednego wątku: zakres całkowania, dokładność i miejsce na wynik
// (definicja w pliku nagłówkowym, bo używają jej dwa pliki: obliczanie_calki.c i dekompozycja_obszaru.c)
struct dane_watku {
  int my_id;          // numer wątku (tylko do wydruku)
  double a_local;     // lewy kraniec podobszaru
  double b_local;     // prawy kraniec podobszaru
  double dx;          // zadana wysokość pojedynczego trapezu
  double wynik;       // tu wątek zapisze całkę ze swojego podobszaru
};

// obliczenie całki przez l_w wątków; dane[i] - zadanie dla wątku i (uzupełnione w main)
double calka_dekompozycja_obszaru(struct dane_watku *dane, int l_w);

#endif
