import java.util.Scanner;


class Histogram_test {

    // Tworzy, uruchamia i czeka na wątki wariantu 4. Zwraca czas w milisekundach.
    static double zmierz_rownolegle(Obraz obraz, int m, int num_threads) {
        int blok = (m + num_threads - 1) / num_threads;      // blok kolumn
        Thread[] NewThr = new Thread[num_threads];

        long startTime = System.nanoTime();                   // początek pomiaru
        for (int i = 0; i < num_threads; i++) {
            int start_kol = i * blok;
            int koniec_kol = Math.min(start_kol + blok, m);
            NewThr[i] = new Thread(new WatekKolumny(i, start_kol, koniec_kol, obraz));
            NewThr[i].start();
        }
        for (int i = 0; i < num_threads; i++) {
            try {
                NewThr[i].join();
            } catch (InterruptedException e) {}
        }
        long endTime = System.nanoTime();                     // koniec: wszystkie wątki skończyły

        return (endTime - startTime) / 1e6;                   // nanosekundy -> milisekundy
    }

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

        // Kilka powtórzeń: pierwsze są wolniejsze, bo JVM dopiero kompiluje gorące pętle
        // do kodu maszynowego. Dlatego na końcu podajemy najlepszy (najkrótszy) czas.
        final int POWTORZENIA = 5;
        double min_sekw = Double.MAX_VALUE;                   // największa możliwa liczba double
        double min_row = Double.MAX_VALUE;

        for (int powt = 1; powt <= POWTORZENIA; powt++) {

            obraz_1.clear_histogram();                        // liczniki muszą zaczynać od zera
            long startTime = System.nanoTime();
            obraz_1.calculate_histogram();                    // wersja sekwencyjna
            long endTime = System.nanoTime();
            double t_sekw = (endTime - startTime) / 1e6;

            obraz_1.clear_hist_parallel();
            double t_row = zmierz_rownolegle(obraz_1, m, num_threads);

            System.out.println("Pomiar " + powt
                    + ": sekwencyjnie " + String.format("%.2f", t_sekw) + " ms"
                    + ", rownolegle " + String.format("%.2f", t_row) + " ms");

            min_sekw = Math.min(min_sekw, t_sekw);
            min_row = Math.min(min_row, t_row);
        }

        System.out.println("Najlepsze czasy: sekwencyjnie " + String.format("%.2f", min_sekw) + " ms"
                + ", rownolegle " + String.format("%.2f", min_row) + " ms"
                + ", przyspieszenie " + String.format("%.2f", min_sekw / min_row));

        obraz_1.compare_histograms();                         // wynik ostatniego powtórzenia
    }

}
