#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>
#include <sys/random.h>   // S2: getrandom – systemowe źródło losowości

#define ILE_MUSZE_WYPIC 3

// S0: zmienna globalna – liczba wolnych kufli, wspólna dla wszystkich
// wątków (klientów).
int l_wkf;

// S1: muteks chroniący dostęp do l_wkf – jeden, wspólny dla wszystkich klientów
pthread_mutex_t muteks_kufli;

void * watek_klient (void * arg);

int main( void ){

  pthread_t *tab_klient;
  int *tab_klient_id;

  int l_kl, l_kf, l_kr, i;
  unsigned int ziarno;   // S2: ziarno generatora liczb losowych

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  // S0: na otwarciu pubu wszystkie kufle są wolne
  l_wkf = l_kf;

  // S1: przygotowanie muteksu do pracy – przed utworzeniem wątków
  pthread_mutex_init(&muteks_kufli, NULL);

  // S2: ustawienie ziarna generatora liczb losowych (raz, przed wątkami).
  // Ziarno pobieramy z systemowego źródła losowości, więc jest inne przy
  // każdym uruchomieniu – także przy kilku uruchomieniach w tej samej sekundzie.
  if(getrandom(&ziarno, sizeof(ziarno), 0) == -1){
    ziarno = time(NULL);   // awaryjnie, gdyby system nie podał liczby losowej
  }
  srand(ziarno);

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
  int sukces;     // S2: 1 – udało się pobrać kufel, 0 – jeszcze nie
  int czekanie;   // S2: wylosowany czas czekania w sekundach
  int ile_musze_wypic = ILE_MUSZE_WYPIC;

  long int wykonana_praca = 0;

  printf("\nKlient %d, wchodzę do pubu\n", moj_id); 
    
  for(i=0; i<ile_musze_wypic; i++){

    printf("\nKlient %d, wybieram kufel\n", moj_id); 

    // S2: aktywne czekanie – klient sam, raz za razem, sprawdza,
    // czy jest wolny kufel, dopóki go nie dostanie
    sukces = 0;
    do{
      pthread_mutex_lock(&muteks_kufli);    // wejście do sekcji krytycznej
      if(l_wkf > 0){                        // czy jest wolny kufel?
        l_wkf = l_wkf - 1;                  // tak – pobranie kufla
        sukces = 1;
      }
      pthread_mutex_unlock(&muteks_kufli);  // wyjście z sekcji krytycznej

      if(sukces == 0){                      // nie było wolnego kufla:
        czekanie = 1 + rand() % 3;          // losowo 1, 2 albo 3 sekundy
        printf("\nKlient %d, brak wolnych kufli, czekam %d s\n",
               moj_id, czekanie);
        sleep(czekanie);                    // czekanie POZA sekcją krytyczną
      }
    } while(sukces == 0);
    
    j=0;
    printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j); 
    usleep(30);
    
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
