#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

void * watek_klient (void * arg);

int main( void ){

  pthread_t *tab_klient; // na identyfikatory wątków
  int *tab_klient_id; // na numery klientów 0, 1, 2…

  int l_kl, l_kf, l_kr, i;

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;


  printf("\nOtwieramy pub (simple)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 

// tworzy wątki w pętli
  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }

// main zasypia dopoki watek sie nie skonczy
// Bez tej pętli main doszedłby do końca i proces by się zakończył. Wszystkie wątki zostałyby wtedy zabite w połowie piwa.
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  printf("\nZamykamy pub!\n");


}


void * watek_klient (void * arg_wsk){
// Wszystkie zmienne klienta są lokalne, a zmiennych globalnych nie ma.


// klient przepisuje swój numer do własnej zmiennej lokalnej.
  int moj_id = * ((int *)arg_wsk);

  int i, j, kufel, result;
  int ile_musze_wypic = ILE_MUSZE_WYPIC;

  long int wykonana_praca = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i=0; i<ile_musze_wypic; i++){

/*
* Najważniejsze spostrzeżenie: „wybieram kufel” i „odkladam kufel” to tylko napisy. Program w ogóle nie śledzi kufli:
* l_kf jest tylko wypisywane,
* zmienna kufel jest nieużywana,
* wykonana_praca zawsze wynosi 0
*/

    printf("\nKlient %d, wybieram kufel\n", moj_id); 
    
    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j); 
    usleep(30);
    
    printf("\nKlient %d, pije\n", moj_id); 
    nanosleep((struct timespec[]){{0, 50000000L}}, NULL);
    
    printf("\nKlient %d, odkladam kufel\n", moj_id); 
    
  }

  printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
	 moj_id, wykonana_praca); 
    
  return(NULL);
} 


