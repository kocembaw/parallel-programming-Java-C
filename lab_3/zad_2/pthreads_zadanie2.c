// pthreads_zadanie2.c
// Przesylanie indywidualnych identyfikatorow 0..N-1 do N watkow - dwa warianty:
//   1) "iterator" - do watku przesylany jest adres zmiennej sterujacej petli (&i)  -> BLEDNY
//   2) "tablica"  - kazdy watek dostaje adres WLASNEJ komorki tablicy (&ident[i]) -> POPRAWNY
// Kazdy watek wypisuje swoj systemowy ID (pthread_self) i otrzymany identyfikator,
// a nastepnie oddaje otrzymany identyfikator watkowi glownemu przez pthread_join.
// Watek glowny porownuje: watek utworzony jako k-ty powinien otrzymac identyfikator k.
//
// kompilacja:   gcc -Wall -pthread pthreads_zadanie2.c -o pthreads_zadanie2   (albo: make)
// uruchomienie: ./pthreads_zadanie2 [N] [iterator|tablica]
//               bez drugiego argumentu wykonywane sa oba warianty, jeden po drugim
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define WARIANT_ITERATOR 1
#define WARIANT_TABLICA  2

void *zadanie_watku(void *arg_wsk)
{
    // od razu kopiujemy przeslana wartosc do zmiennej lokalnej (prywatnej - na stosie watku)
    int moj_id = *((int *) arg_wsk);

    printf("Watek: systemowy ID = %lu, przeslany identyfikator = %d\n",
           (unsigned long) pthread_self(), moj_id);

    // oddajemy otrzymany identyfikator watkowi glownemu jako wartosc zwracana;
    // liczbe zapisujemy wprost w miejscu wskaznika (rzutowanie przez intptr_t),
    // dzieki temu nie trzeba alokowac pamieci na wynik
    return (void *) (intptr_t) moj_id; //wpisuje samą liczbę w miejsce adresu
}

// tworzy N watkow wybranym sposobem, czeka na nie i sprawdza otrzymane identyfikatory;
// zwraca liczbe watkow, ktore dostaly zly identyfikator
int test(int n, int wariant)
{
    pthread_t *tid       = malloc(n * sizeof(pthread_t));
    int       *ident     = malloc(n * sizeof(int));        // dedykowana tablica identyfikatorow
    int       *otrzymany = malloc(n * sizeof(int));        // co faktycznie dostal k-ty watek
    int       *ile_razy  = calloc(n + 1, sizeof(int));     // ile watkow dostalo dana wartosc
    int i;

    printf("\n========== WARIANT: %s, N = %d ==========\n",
           wariant == WARIANT_ITERATOR ? "ITERATOR PETLI (&i)" : "DEDYKOWANA TABLICA (&ident[i])", n);

    // --- tworzenie watkow ---
    for (i = 0; i < n; i++) {
        if (wariant == WARIANT_ITERATOR) {
            // BLAD: wszystkie watki dostaja adres tej samej zmiennej i,
            // ktora watek glowny zaraz zmieni (i++, a pozniej petla join)
            pthread_create(&tid[i], NULL, zadanie_watku, &i);
        } else {
            // POPRAWNIE: kazdy watek dostaje adres swojej komorki,
            // ktorej wartosc juz sie nie zmieni
            ident[i] = i;
            pthread_create(&tid[i], NULL, zadanie_watku, &ident[i]);
        }
    }

    // --- oczekiwanie na zakonczenie i odbior wynikow ---
    for (i = 0; i < n; i++) {
        void *wynik;
        pthread_join(tid[i], &wynik);
        otrzymany[i] = (int) (intptr_t) wynik;
    }

    // --- sprawdzenie: watek utworzony jako k-ty powinien dostac identyfikator k ---
    int bledne = 0;
    printf("\n--- sprawdzenie (watek utworzony jako k-ty powinien dostac k) ---\n");
    for (i = 0; i < n; i++) {
        int ok = (otrzymany[i] == i);
        if (!ok) bledne++;
        if (otrzymany[i] >= 0 && otrzymany[i] <= n) ile_razy[otrzymany[i]]++;
        printf("watek nr %2d: oczekiwany %2d, otrzymany %2d  %s\n",
               i, i, otrzymany[i], ok ? "OK" : "<-- BLAD");
    }

    // identyfikatory, ktore dostal wiecej niz jeden watek
    printf("powtorzone identyfikatory:");
    int powtorzone = 0;
    for (i = 0; i <= n; i++) {
        if (ile_razy[i] > 1) { printf(" %d (x%d)", i, ile_razy[i]); powtorzone++; }
    }
    printf("%s\n", powtorzone ? "" : " brak");

    // identyfikatory z zakresu 0..N-1, ktorych nie dostal zaden watek
    printf("brakujace identyfikatory:");
    int brakujace = 0;
    for (i = 0; i < n; i++) {
        if (ile_razy[i] == 0) { printf(" %d", i); brakujace++; }
    }
    printf("%s\n", brakujace ? "" : " brak");

    printf("WYNIK: %d z %d watkow otrzymalo zly identyfikator -> %s\n",
           bledne, n, bledne ? "PRZESYLANIE BLEDNE" : "PRZESYLANIE POPRAWNE");

    free(tid);
    free(ident);      // dopiero po join - watki juz nie korzystaja z tablicy
    free(otrzymany);
    free(ile_razy);
    return bledne;
}

int main(int argc, char *argv[])
{
    int n = 8;
    if (argc > 1) n = atoi(argv[1]);
    if (n < 1) n = 1;

    if (argc > 2 && strcmp(argv[2], "iterator") == 0) {
        test(n, WARIANT_ITERATOR);
    } else if (argc > 2 && strcmp(argv[2], "tablica") == 0) {
        test(n, WARIANT_TABLICA);
    } else {
        test(n, WARIANT_ITERATOR);
        test(n, WARIANT_TABLICA);
    }
    return 0;
}
