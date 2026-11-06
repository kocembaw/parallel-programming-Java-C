import java.util.Scanner;


class Histogram_test {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#kolumns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        // W wariancie 2 liczba symboli i liczba wątków są niezależne
        System.out.println("Set number of symbols (1-94)");
        int liczba_znakow = scanner.nextInt();
        if (liczba_znakow < 1 || liczba_znakow > 94) {
            System.out.println("Liczba symboli musi byc z zakresu 1-94");
            return;
        }

        System.out.println("Set number of threads");
        int num_threads = scanner.nextInt();
        if (num_threads < 1) {
            System.out.println("Liczba watkow musi byc co najmniej 1");
            return;
        }

        Obraz obraz_1 = new Obraz(n, m, liczba_znakow);

        // wersja sekwencyjna - wzorzec do porównania
        obraz_1.calculate_histogram();
        obraz_1.print_histogram();
        System.out.print("\n");

        // podział blokowy: rozmiar bloku = sufit(liczba_znakow / num_threads)
        int blok = (liczba_znakow + num_threads - 1) / num_threads;

        Thread[] NewThr = new Thread[num_threads];   // tym razem tablica zwykłych obiektów Thread

        // 1) tworzymy wątki i wypisujemy przydział - żaden wątek jeszcze nie działa
        for (int i = 0; i < num_threads; i++) {
            int start_znak = i * blok;                                     // początek bloku wątku i
            int koniec_znak = Math.min(start_znak + blok, liczba_znakow);  // ostatni blok może być krótszy

            if (start_znak < koniec_znak)
                System.out.println("Przydzial: watek " + i + " -> symbole o indeksach "
                                   + start_znak + ".." + (koniec_znak - 1));
            else
                System.out.println("Przydzial: watek " + i + " -> brak symboli");

            WatekBlok zadanie = new WatekBlok(i, start_znak, koniec_znak, obraz_1);  // praca do wykonania
            NewThr[i] = new Thread(zadanie);                                       // wątek, który ją wykona
        }
        System.out.print("\n");

        // 2) uruchamiamy wszystkie wątki
        for (int i = 0; i < num_threads; i++) {
            NewThr[i].start();
        }

        // 3) czekamy na zakończenie wszystkich wątków
        for (int i = 0; i < num_threads; i++) {
            try {
                NewThr[i].join();                    // czekaj, aż wątek i skończy
            } catch (InterruptedException e) {}
        }

        // wszystkie wątki skończyły - można porównać wyniki
        obraz_1.compare_histograms();
    }

}
