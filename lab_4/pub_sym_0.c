#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

// S0: zmienna globalna – liczba wolnych kufli, wspólna dla wszystkich
// wątków (klientów). Dostęp do niej NIE jest w żaden sposób zabezpieczony.
int l_wkf;

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

  //Jeśli jeden klient zostanie zatrzymany między odczytem a zapisem, a drugi w tym czasie zmieni licznik, jedna zmiana przepadnie.
  // przeplatają się będą kroki operacji liczba_kufli = liczba_kufli - 1 (odczyt, odjęcie, zapis)
  printf("\nLiczba wolnych kufli na koniec %d\n", l_wkf);

  /* Znak różnicy:
   *dodatnia znaczy, że przepadło pobranie kufla (kufle „się rozmnożyły”),
   *ujemna znaczy, że przepadło oddanie (kufle „zniknęły”).
  */

}


void * watek_klient (void * arg_wsk){

  int moj_id = * ((int *)arg_wsk);

  int i, j, kufel, result;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;

  long int wykonana_praca = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i=0; i<ile_musze_wypic; i++){

    printf("\nKlient %d, wybieram kufel\n", moj_id); 
    l_wkf = l_wkf - 1;  // S0: pobranie kufla – bez zabezpieczenia
    
    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j); 
    usleep(30);
    
    printf("\nKlient %d, pije\n", moj_id); 
    nanosleep((struct timespec[]){{0, 50000000L}}, NULL);
    
    printf("\nKlient %d, odkladam kufel\n", moj_id); 
    l_wkf = l_wkf + 1;  // S0: oddanie kufla – bez zabezpieczenia
    
  }

  printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
	 moj_id, wykonana_praca); 
    
  return(NULL);
}
