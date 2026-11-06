import java.util.Scanner;


class Histogram_test {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#kolumns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        // Liczbę wątków podajemy PRZED utworzeniem obrazu,
        // bo od niej zależy, z ilu symboli losujemy znaki.
        System.out.println("Set number of threads (1-94) = liczba znakow");
        int num_threads = scanner.nextInt();
        if (num_threads < 1 || num_threads > 94) {
            System.out.println("Liczba watkow musi byc z zakresu 1-94");
            return;                                  // koniec programu
        }

        Obraz obraz_1 = new Obraz(n, m, num_threads);

        // wersja sekwencyjna - wzorzec do porównania
        obraz_1.calculate_histogram();
        obraz_1.print_histogram();
        System.out.print("\n");

        // wersja równoległa: wątek nr i liczy symbol nr i
        Watek[] NewThr = new Watek[num_threads];     // tablica na referencje do wątków

        for (int i = 0; i < num_threads; i++) {
            (NewThr[i] = new Watek(i, obraz_1)).start();   // utwórz wątek i uruchom go
        }

        for (int i = 0; i < num_threads; i++) {
            try {
                NewThr[i].join();                    // czekaj, aż wątek i skończy
            } catch (InterruptedException e) {}
        }

        // wszystkie wątki skończyły - można porównać wyniki
        obraz_1.compare_histograms();
    }

}
