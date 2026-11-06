#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<math.h>

#include"pomiar_czasu.h"

#ifndef ROZMIAR
#define ROZMIAR 100000000 
#endif

#ifndef LICZBA_W
#define LICZBA_W 2 // lepiej: stała - LICZBA_W_MAX i parametr p - liczba wątków 
#endif

pthread_mutex_t muteks; 
pthread_t watki[LICZBA_W];
double global_array_of_local_sums[LICZBA_W];


void *suma_w( void *arg_wsk);
void *suma_w_no_mutex( void *arg_wsk);

double *tab;
double suma=0; 

int main( int argc, char *argv[] ){

  int i; 

  double t1,t2,t3;

  int indeksy[LICZBA_W]; 
  for(i=0;i<LICZBA_W;i++) indeksy[i]=i;

  tab = (double *) malloc(ROZMIAR*sizeof(double));
  for(i=0; i<ROZMIAR; i++ ) tab[i] = ((double) i+1) / ROZMIAR; 

  printf("Obliczenia sekwencyjne\n");
  t1 = czas_zegara();
  for( i = 0; i < ROZMIAR; i++){ 
    suma += tab[i]; 
  }
  t1 = czas_zegara() - t1;
  printf("suma = %lf\n", suma);
  printf("Czas obliczen sekwencyjnych = %lf\n", t1);


  pthread_mutex_init( &muteks, NULL);

  printf("Poczatek tworzenia watkow\n");
  t1 = czas_zegara();

  suma =0;
  for(i=0; i<LICZBA_W; i++ ) 
    pthread_create( &watki[i], NULL, suma_w, (void *) &indeksy[i] );
  // code below - synchronization error !!!
    // pthread_create( &watki[i], NULL, suma_w, (void *) &i ); 

  for(i=0; i<LICZBA_W; i++ ) pthread_join( watki[i], NULL );

  t1 = czas_zegara() - t1;
  printf("suma = %lf\n", suma);
  printf("Czas obliczen %d wątków = %lf\n", LICZBA_W, t1);

  // version with array of local sums (to eliminate the mutex) 
  suma =0;
  printf("Poczatek tworzenia watkow\n");
  t1 = czas_zegara();

  for(i=0; i<LICZBA_W; i++ ) {
    global_array_of_local_sums[i]=0.0;
    pthread_create( &watki[i], NULL, suma_w_no_mutex, (void *) &indeksy[i] );
  // synchronization error !!!
    //pthread_create( &watki[i], NULL, suma_w, (void *) &i );
  }

  for(i=0; i<LICZBA_W; i++ ) {
    pthread_join( watki[i], NULL );
    suma += global_array_of_local_sums[i];
  }

  t1 = czas_zegara() - t1;
  printf("suma = %lf\n", suma);
  printf("Czas obliczen %d wątków (globalna tablica zamiast mutex'a) = %lf\n",
	 LICZBA_W, t1);

/*   suma =0; */

/*   printf("Poczatek tworzenia watkow OpenMP\n"); */
/*   t1 = czas_zegara(); */

/* #pragma omp parallel for num_threads(LICZBA_W) default(none) shared (tab) reduction(+:suma)  */
/*   for(i=0;i<ROZMIAR;i++){ */
/*     suma += tab[i]; */
/*   } */



/*   t1 = czas_zegara() - t1; */
/*   printf("suma = %lf\n", suma); */
/*   printf("Czas obliczen OpenMP = %lf\n", t1); */


 
}

void *suma_w( void *arg_wsk){

  int i, j, moj_id;
  int poczatek, koniec;
  double moja_suma=0;

  moj_id = *( (int *) arg_wsk ); 

  // co w przypadku jeśli ROZMIAR jest niepodzielne przez LICZBA_W i następuje
  // zaokrąglenie w górę? - jak zagwarantować nieprzekroczenie w wersji
  // równoległej górnej granicy iteracji N?
  //if(j*LICZBA_W!=ROZMIAR) { printf("Error! Exiting.\n"); exit(0);}

  //j=ceil( (float)ROZMIAR/LICZBA_W );  // float ma ok. 7 cyfr znaczących - dla dużego ROZMIAR blok może wyjść za mały

  // rozmiar bloku zaokrąglony w górę, liczony na liczbach całkowitych (bez float)
  j = (ROZMIAR + LICZBA_W - 1) / LICZBA_W;

  // zakres indeksów tego wątku
  poczatek = j*moj_id;
  koniec   = j*(moj_id+1);
  // ostatni blok może wystawać poza tablicę - przycinamy go do ROZMIAR
  if(koniec > ROZMIAR) koniec = ROZMIAR;


  // Sposób A – zmienna globalna i muteks (sekcja krytyczna)
  for( i = poczatek; i < koniec; i++){   // pętla po PRZYCIĘTYM zakresie
    moja_suma += tab[i];  // prywatna suma wątku - bez synchronizacji
  }

  pthread_mutex_lock( &muteks ); // tylko jeden wątek naraz
  suma += moja_suma;
  pthread_mutex_unlock( &muteks );

  pthread_exit( NULL );

}

// sposob B: tablica sum lokalnych
// Każdy wątek sumuje swój blok tablicy tab i zapisuje wynik do własnej komórki
// global_array_of_local_sums[moj_id]. Końcową sumę liczy main, po pthread_join.

void *suma_w_no_mutex( void *arg_wsk){

  int i, j, moj_id;
  int poczatek, koniec;

  moj_id = *( (int *) arg_wsk ); 

  double tmp=0.0;
  //j=ceil( (float)ROZMIAR/LICZBA_W ); // could be double as well 

  // ten sam podział co w suma_w: rozmiar bloku w górę (liczby całkowite) i przycięcie końca
  j = (ROZMIAR + LICZBA_W - 1) / LICZBA_W;
  poczatek = j*moj_id;
  koniec   = j*(moj_id+1);
  if(koniec > ROZMIAR) koniec = ROZMIAR;

  for( i = poczatek; i < koniec; i++){ 
    // False sharing występuje wtedy, gdy wątki zapisują do różnych zmiennych, które leżą w tym samym kawałku pamięci podręcznej
    //global_array_of_local_sums[moj_id] += tab[i];
    // Procesor pilnuje, żeby żaden rdzeń nie liczył na nieaktualnej kopii. Gdy jeden rdzeń zapisze coś w kawałku, kopia tego kawałka u 
    // drugiego rdzenia przestaje być ważna. Drugi rdzeń musi wtedy pobrać aktualną wersję, zanim sam coś w nim zapisze.
    // better - probably would be employed by the compiler anyway
    tmp+=tab[i];
  }

  // no critical section, but requires an array with length nr_threads
  global_array_of_local_sums[moj_id] = tmp;

  pthread_exit( (void *)0);

}  
