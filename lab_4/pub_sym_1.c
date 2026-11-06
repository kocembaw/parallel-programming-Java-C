#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

// S0: zmienna globalna – liczba wolnych kufli, wspólna dla wszystkich
// wątków (klientów).
int l_wkf;

// S1: muteks chroniący dostęp do l_wkf – jeden, wspólny dla wszystkich klientów
pthread_mutex_t muteks_kufli; // Typ zmiennej przechowującej muteks, czyli „zamek z kluczem”.
pthread_mutex_t muteks_kranu;

void * watek_klient (void * arg);

int main( void ){

  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  // S0: na otwarciu pubu wszystkie kufle są wolne
  l_wkf = l_kf;

  // S1: przygotowanie muteksu do pracy - przed utworzerzeniem wątku ("montuje zamek i wiesza przy drzwiach")
  // muteksu nie wolno używać przed inicjalizacją
  pthread_mutex_init(&muteks_kufli, NULL); //& pojawia się dlatego, że funkcja musi zmienić naszą zmienną

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;


  printf("\nOtwieramy pub (simple)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  printf("\nZamykamy pub!\n");

  // S0: liczba kufli na zakończenie symulacji (wszyscy klienci już wyszli,
  // więc powinna być równa liczbie z początku)
  printf("\nLiczba wolnych kufli na koniec %d\n", l_wkf);


}


void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);

  int i, j, kufel, result;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;

  long int wykonana_praca = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i=0; i<ile_musze_wypic; i++){

    printf("\nKlient %d, wybieram kufel\n", moj_id);
   
    // Klient czekający w lock nie zużywa procesora: system go usypia i budzi, gdy klucz się zwolni. 
    pthread_mutex_lock(&muteks_kufli);    // S1: wejście do sekcji krytycznej "weź klucz"
    l_wkf = l_wkf - 1;                    // pobranie kufla
    pthread_mutex_unlock(&muteks_kufli);  // S1: wyjście z sekcji krytycznej "odwieś klucz", Jeśli ktoś czekał, jeden z czekających dostaje go
    
    /*Planista nadal może przerwać wątek w środku sekcji krytycznej, między odczytem a zapisem l_wkf. Różnica polega na tym, że przerwany 
     * wątek ma przy sobie klucz
     */

    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j);
    pthread_mutex_lock(&muteks_kranu); 
    usleep(30);
    pthread_mutex_unlock(&muteks_kranu);
    
    printf("\nKlient %d, pije\n", moj_id); 
    nanosleep((struct timespec[]){{0, 50000000L}}, NULL);
    
    printf("\nKlient %d, odkladam kufel\n", moj_id); 
    pthread_mutex_lock(&muteks_kufli);    // S1: wejście do sekcji krytycznej
    l_wkf = l_wkf + 1;                    // oddanie kufla
    pthread_mutex_unlock(&muteks_kufli);  // S1: wyjście z sekcji krytycznej
    
  }

  printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
	 moj_id, wykonana_praca); 
    
  return(NULL);
} 


