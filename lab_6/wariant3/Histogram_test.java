import java.util.Scanner;


class Histogram_test {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#kolumns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

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
        System.out.println("Histogram sekwencyjny:");
        obraz_1.calculate_histogram();
        obraz_1.print_histogram();
        System.out.print("\n");

        Thread[] NewThr = new Thread[num_threads];

        // 1) tworzymy wątki i wypisujemy przydział wierszy (podział cykliczny)
        for (int i = 0; i < num_threads; i++) {
            System.out.print("Przydzial: watek " + i + " -> wiersze: [");
            for (int w = i; w < n; w += num_threads) {      // te same wiersze, które policzy wątek
                System.out.print(" " + w);
            }
            System.out.print(" ]\n");                       // nawias: linia nie kończy się na " 0"

            NewThr[i] = new Thread(new WatekWiersze(i, num_threads, obraz_1));
        }
        System.out.print("\n");

        // 2) uruchamiamy wszystkie wątki
        for (int i = 0; i < num_threads; i++) {
            NewThr[i].start();
        }

        // 3) czekamy na zakończenie wszystkich wątków
        for (int i = 0; i < num_threads; i++) {
            try {
                NewThr[i].join();
            } catch (InterruptedException e) {}
        }

        // 4) drukuje wątek główny - wszystkie wątki już skończyły
        System.out.println("\nHistogram rownolegly (hist_parallel):");
        obraz_1.print_hist_parallel();
        System.out.print("\n");

        obraz_1.compare_histograms();
    }

}
