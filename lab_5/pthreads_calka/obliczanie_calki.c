k#include<stdio.h>
#include<math.h>
#include"pomiar_czasu.h"
#include"dekompozycja_obszaru.h"   // struct dane_watku i prototyp calka_dekompozycja_obszaru

#ifndef M_PI // standardy C99 i C11 nie wymagają definiowania stałej M_PI
#define M_PI (3.14159265358979323846)
#endif


double funkcja ( double x );

double funkcja ( double x ){ return( sin(x) ); }

double calka_sekw(double a, double b, double dx);

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w);



int main( int argc, char *argv[] ){

  int i; 

  double t1,t2,t3;

  double a, b, dx, calka;

  printf("\nProgram obliczania całki z funkcji (sinus) metodą trapezów.\n");

  a = 0.0;
  //printf("\nPodaj lewy kraniec przedziału całkowania: "); scanf("%lf", &a);

  b = M_PI;
  //printf("\nPodaj prawy kraniec przedziału całkowania: "); scanf("%lf", &b);

  printf("\nPodaj wysokość pojedynczego trapezu:  "); scanf("%lf", &dx);

  int l_w=0;
  printf("\nPodaj liczbę wątków:  "); scanf("%d", &l_w);
  if(l_w < 1){ printf("\nLiczba wątków musi być co najmniej 1\n"); return 1; }

  printf("\nPoczatek obliczeń sekwencyjnych\n");
  t1 = czas_zegara();

  calka = calka_sekw(a, b, dx);

  t1 = czas_zegara() - t1;
  printf("\nKoniec obliczen sekwencyjnych\n");
  printf("\tCzas wykonania %lf. \tObliczona całka = %.15lf\n", t1, calka);

  printf("\nPoczatek obliczeń równoległych (zrównoleglenie pętli)\n");
  t1 = czas_zegara();

  calka = calka_zrownoleglenie_petli(a, b, dx, l_w);

  t1 = czas_zegara() - t1;
  printf("\nKoniec obliczen  równoległych (zrównoleglenie pętli) \n");
  printf("\tCzas wykonania %lf. \tObliczona całka = %.15lf\n", t1, calka);

  // dekompozycja obszaru: main dzieli obszar [a, b] na l_w równych podobszarów
  // i uzupełnia obiekty struktury z zakresami całkowania - po jednym dla każdego wątku
  struct dane_watku dane[l_w];
  double szerokosc = (b - a) / l_w;            // długość podobszaru jednego wątku
  for(i=0; i<l_w; i++){
    dane[i].my_id = i;
    dane[i].a_local = a + i*szerokosc;         // początek podobszaru i
    dane[i].b_local = a + (i+1)*szerokosc;     // koniec podobszaru i = początek podobszaru i+1
    dane[i].dx = dx;                           // ta sama zadana dokładność dla wszystkich
    dane[i].wynik = 0.0;                       // tu wątek wpisze swój wynik
  }
  dane[l_w-1].b_local = b;   // ostatni podobszar kończy się dokładnie w b (bez błędu zaokrąglenia)

  printf("\nPoczatek obliczeń równoległych (dekompozycja obszaru)\n");
  t1 = czas_zegara();

  calka = calka_dekompozycja_obszaru(dane, l_w);

  t1 = czas_zegara() - t1;
  printf("\nKoniec obliczen  równoległych (dekompozycja obszaru) \n");
  printf("\tCzas wykonania %lf. \tObliczona całka = %.15lf\n", t1, calka);

}

double calka_sekw(double a, double b, double dx){

  int N = ceil((b-a)/dx); // ile trapezów; zaokrąglenie w górę
  double dx_adjust = (b-a)/N;

  printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);
  //printf("a %lf, b %lf, n %d, dx %.12lf (dx_adjust %.12lf)\n", a, b, N, dx, dx_adjust);
  int i;
  double calka = 0.0;
  for(i=0; i<N; i++){

    double x1 = a + i*dx_adjust;
    calka += 0.5*dx_adjust*(funkcja(x1)+funkcja(x1+dx_adjust));
    //printf("i %d, x1 %lf, funkcja(x1) %lf, całka = %.15lf\n", 
    //	   i, x1, funkcja(x1), calka);

  }

  return(calka);
}
