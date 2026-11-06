// pthreads_zadanie3.c
// Przesylanie do watkow wskaznika do struktury zawierajacej pola ROZNYCH typow
// (int, tablica znakow, double). Pola wejsciowe (in) wypelnia watek glowny,
// pole wynikowe (in/out) watek odczytuje i nadpisuje swoim wynikiem.
//
// Zadanie watku (szkielet pod lab 5 - calka w przedziale (a,b)):
//   przedzial (a,b) dzielony jest na N rownych fragmentow, kazdy watek dostaje
//   swoj fragment i liczy jego srodek; watek glowny po zakonczeniu watkow
//   sumuje wyniki ze wszystkich struktur.
//
// kompilacja:   gcc -Wall -pthread pthreads_zadanie3.c -o pthreads_zadanie3   (albo: make)
// uruchomienie: ./pthreads_zadanie3 [N] [a] [b]      (domyslnie N=4, a=0, b=1)
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// struktura z danymi dla jednego watku - trzy rozne typy pol
typedef struct {
    int    id;           // in     - numer watku 0..N-1                  (int)
    char   nazwa[24];    // in     - etykieta tekstowa, np. "watek_2"     (tablica znakow)
    double a_lok;        // in     - poczatek fragmentu przedzialu        (double)
    double b_lok;        // in     - koniec fragmentu przedzialu          (double)
    double wynik;        // in/out - main wpisuje 0.0, watek dopisuje wynik (double)
} dane_watku_t;

void *zadanie_watku(void *arg_wsk)
{
    // 1) rzutowanie: wiemy, ze pod adresem lezy struktura dane_watku_t
    dane_watku_t *wsk = (dane_watku_t *) arg_wsk;

    // 2) kopia pol wejsciowych do zmiennej lokalnej (prywatnej - na stosie watku)
    dane_watku_t moje = *wsk;

    // 3) "obliczenie" - srodek fragmentu (w lab 5 tu bedzie calka z f(x) na [a_lok, b_lok])
    double srodek = (moje.a_lok + moje.b_lok) / 2.0;

    // 4) zapis wyniku przez WSKAZNIK - do struktury w pamieci watku glownego
    //    (pole in/out: odczytujemy wartosc poczatkowa i dodajemy do niej wynik)
    wsk->wynik = wsk->wynik + srodek;

    printf("%-8s (id = %d, systemowy ID = %lu): fragment [%8.4f, %8.4f] -> srodek = %8.4f\n",
           moje.nazwa, moje.id, (unsigned long) pthread_self(),
           moje.a_lok, moje.b_lok, srodek);

    return NULL;
}

int main(int argc, char *argv[])
{
    int    n = 4;
    double a = 0.0, b = 1.0;
    if (argc > 1) n = atoi(argv[1]);
    if (argc > 3) { a = atof(argv[2]); b = atof(argv[3]); }
    if (n < 1) n = 1;

    pthread_t    *tid  = malloc(n * sizeof(pthread_t));
    dane_watku_t *dane = malloc(n * sizeof(dane_watku_t));   // OSOBNA struktura dla kazdego watku

    double dx = (b - a) / n;    // dlugosc jednego fragmentu

    printf("Watek glowny: przedzial (%g, %g), %d watkow, dlugosc fragmentu %g\n\n", a, b, n, dx);

    // --- wypelnienie struktur i tworzenie watkow ---
    for (int i = 0; i < n; i++) {
        dane[i].id = i;
        snprintf(dane[i].nazwa, sizeof(dane[i].nazwa), "watek_%d", i);
        dane[i].a_lok = a + i * dx;
        dane[i].b_lok = (i == n - 1) ? b : a + (i + 1) * dx;   // ostatni konczy sie dokladnie w b
        dane[i].wynik = 0.0;                                    // wartosc poczatkowa pola in/out

        pthread_create(&tid[i], NULL, zadanie_watku, &dane[i]); // przesylamy ADRES struktury
    }

    // --- oczekiwanie na zakonczenie wszystkich watkow ---
    for (int i = 0; i < n; i++) {
        pthread_join(tid[i], NULL);
    }

    // --- dopiero teraz pola wynik sa na pewno wypelnione: odczyt i sumowanie ---
    printf("\nWatek glowny: zawartosc struktur po zakonczeniu watkow\n");
    printf("  id | nazwa    |    a_lok |    b_lok |    wynik\n");
    printf("  ---+----------+----------+----------+---------\n");
    double suma = 0.0;
    for (int i = 0; i < n; i++) {
        printf("  %2d | %-8s | %8.4f | %8.4f | %8.4f\n",
               dane[i].id, dane[i].nazwa, dane[i].a_lok, dane[i].b_lok, dane[i].wynik);
        suma += dane[i].wynik;
    }

    double kontrola = n * (a + b) / 2.0;   // suma srodkow n rownych fragmentow = n*(a+b)/2
    printf("\nWatek glowny: suma wynikow       = %.6f\n", suma);
    printf("Watek glowny: kontrola n*(a+b)/2 = %.6f\n", kontrola);

    free(dane);   // dopiero po join - watki juz nie korzystaja ze struktur
    free(tid);
    return 0;
}
