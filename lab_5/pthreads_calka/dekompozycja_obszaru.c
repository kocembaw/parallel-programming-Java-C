k#include<stdio.h>
#include<pthread.h>
#include<math.h>

double funkcja ( double x );

double calka_sekw(double a, double b, double dx);   // prototyp funkcji z obliczanie_calki.c - wątki ją wywołują

double calka_dekompozycja_obszaru(double a, double b, double dx, int l_w);

void* calka_podobszar_w(void* arg_wsk);

// struktura z zadaniem dla jednego wątku: zakres całkowania, dokładność i miejsce na wynik
struct dane_watku {
  int my_id;          // numer wątku (tylko do wydruku)
  double a_local;     // lewy kraniec podobszaru
  double b_local;     // prawy kraniec podobszaru
  double dx;          // zadana wysokość pojedynczego trapezu
  double wynik;       // tu wątek zapisze całkę ze swojego podobszaru
};

double calka_dekompozycja_obszaru(double a, double b, double dx, int l_w){


  //printf("a %lf, b %lf, dx %lf\n", a, b, dx);

  double calka_suma_local = 0.0;

  // tworzenie struktur danych do obsługi wielowątkowości
  pthread_t watki[l_w];            // identyfikatory wątków
  struct dane_watku dane[l_w];     // osobna struktura (zadanie) dla każdego wątku

  double szerokosc = (b - a) / l_w;   // długość podobszaru jednego wątku

  // uzupełnienie struktur: wątek główny dzieli obszar [a, b] na l_w równych podobszarów
  int i;
  for(i=0; i<l_w; i++){
    dane[i].my_id = i;
    dane[i].a_local = a + i*szerokosc;        // początek podobszaru i
    dane[i].b_local = a + (i+1)*szerokosc;    // koniec podobszaru i = początek podobszaru i+1
    dane[i].dx = dx;                          // ta sama zadana dokładność dla wszystkich
    dane[i].wynik = 0.0;
  }
  dane[l_w-1].b_local = b;   // ostatni podobszar kończy się dokładnie w b (bez błędu zaokrąglenia)


  // tworzenie wątków - każdy dostaje adres SWOJEJ struktury
  // na 4 argumencieprzekazujemy adres struktury watku
  for(i=0; i<l_w; i++){
    pthread_create( &watki[i], NULL, calka_podobszar_w, (void *) &dane[i] );
  }


  // oczekiwanie na zakończenie pracy wątków i zebranie wyników
  for(i=0; i<l_w; i++){
    pthread_join( watki[i], NULL );
    calka_suma_local += dane[i].wynik;   // po pthread_join wynik wątku i na pewno jest gotowy
  }

  return(calka_suma_local);
}



void* calka_podobszar_w(void* arg_wsk){

  double a_local, b_local, dx;
  // rozpakowanie danych przesłanych do wątku
  struct dane_watku *moje_dane = (struct dane_watku *) arg_wsk;   // arg_wsk wskazuje na strukturę tego wątku
  a_local = moje_dane->a_local;
  b_local = moje_dane->b_local;
  dx = moje_dane->dx;

  int my_id = moje_dane->my_id; // skąd pobierany? - z pola struktury
  printf("\nWątek %d: a_local %lf, b_local %lf, dx %lf\n", 
	 my_id, a_local, b_local, dx);

  // wywołanie zadania do wykonania: całkowanie w zadanym przedziale 
  double calka = calka_sekw(a_local, b_local, dx);

  // przekazanie wyniku: zapis do pola struktury (każdy wątek pisze do swojej struktury - bez muteksu)
  moje_dane->wynik = calka;

  pthread_exit( NULL );
}
