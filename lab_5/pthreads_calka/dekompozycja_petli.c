#include<stdio.h>
#include<pthread.h>
#include<math.h>

double funkcja ( double x );

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w);

static int l_w_global=0;

static double calka_global=0.0;
static double a_global;
static double b_global;
static double dx_global;
static int N_global;

static pthread_mutex_t muteks;   // pilnuje, żeby tylko jeden wątek naraz zmieniał calka_global

void* calka_fragment_petli_w(void* arg_wsk);

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w){

  int N = ceil((b-a)/dx); // zaokraglamy w gore, nie w dol, bo wtedy kawalek odcinka zostalby niepoliczony
  
  // teraz zaś drugi problem: za dużo trapezow i wychodzimy poza odcinek - zmniejszamy szerokosc trapezu i 32 trapezy mieszczą sie
  double dx_adjust = (b-a)/N; 

  printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);
  //printf("a %lf, b %lf, n %d, dx %.12lf (dx_adjust %.12lf)\n", a, b, N, dx, dx_adjust);


  // tworzenie struktur danych do obsługi wielowątkowości

  // dane wspólne dla wszystkich wątków - wątki odczytają je ze zmiennych globalnych
  a_global = a;
  b_global = b;
  dx_global = dx_adjust;   // ta sama poprawiona szerokość trapezu co w wersji sekwencyjnej
  N_global = N;            // ta sama liczba trapezów co w wersji sekwencyjnej
  l_w_global = l_w;
  calka_global = 0.0;      // tu wątki będą dodawać swoje wyniki częściowe

  pthread_t watki[l_w];    // identyfikatory wątków
  int indeksy[l_w];        // numery wątków 0, 1, ..., l_w-1 - każdy wątek dostaje adres swojej komórki

  pthread_mutex_init( &muteks, NULL );


  // tworzenie wątków
  int i;
  for(i=0; i<l_w; i++){
    indeksy[i] = i;
    pthread_create( &watki[i], NULL, calka_fragment_petli_w, (void *) &indeksy[i] );
  }


  // oczekiwanie na zakończenie pracy wątków
  for(i=0; i<l_w; i++){
    pthread_join( watki[i], NULL );
  }

  pthread_mutex_destroy( &muteks );   // muteks nie jest już potrzebny

  return(calka_global);
}

void* calka_fragment_petli_w(void* arg_wsk){

  int my_id = *( (int *) arg_wsk );   // numer wątku przekazany przez pthread_create

  double a, b, dx; // skąd pobrać dane a, b, dx, N, l_w ? 
  int N, l_w;      // wariant 1 - globalne

  // kopiujemy wartości globalne do zmiennych lokalnych (prywatnych) wątku
  a = a_global;
  b = b_global;
  dx = dx_global;
  N = N_global;
  l_w = l_w_global;

  // dekompozycja cykliczna: wątek liczy trapezy my_id, my_id+l_w, my_id+2*l_w, ...
  //int my_start = my_id; // pierwszy trapez = numer wątku
  //int my_end = N;  // liczymy aż do końca (do trapezu N-1)
  //int my_stride = l_w; // skok = liczba wątków

  // dekompozycja blokowa: wątek liczy jeden ciągły kawałek (blok) trapezów
  int my_size = (N + l_w - 1) / l_w;    // rozmiar bloku: N/l_w zaokrąglone w górę Dodanie l_w - 1 „przepycha” resztę tak, że to obcięcie staje
                                        // sie zaokragleniem w gore

  int my_start = my_id * my_size;       // pierwszy trapez bloku
  int my_end = (my_id + 1) * my_size;   // pierwszy trapez ZA blokiem (początek następnego bloku)
  if(my_end > N) my_end = N;            // ostatni blok przycinamy, żeby nie wyszedł poza N (bloki razem pokrywają więcej miejscniżjesttrapezow
  int my_stride = 1;                    // trapezy po kolei, bez przeskakiwania

  // something else ? (dekompozycja blokowo-cykliczna)

  printf("\nWątek %d: my_start %d, my_end %d, my_stride %d\n", 
	 my_id, my_start, my_end, my_stride);


  int i;
  double calka = 0.0;   // wynik częściowy tego wątku (zmienna prywatna)
  for(i=my_start; i<my_end; i+=my_stride){

    double x1 = a + i*dx;  // lewy brzeg trapezu nr i

    calka += 0.5*dx*(funkcja(x1)+funkcja(x1+dx)); // dodaj pole trapezu nr i

    //printf("i %d, x1 %lf, funkcja(x1) %lf, całka = %.15lf\n", 
    //	   i, x1, funkcja(x1), calka);

  }

  // sekcja krytyczna: dodanie wyniku częściowego do wspólnej sumy
  pthread_mutex_lock( &muteks );
  calka_global += calka;
  pthread_mutex_unlock( &muteks );

  pthread_exit( NULL );
}
