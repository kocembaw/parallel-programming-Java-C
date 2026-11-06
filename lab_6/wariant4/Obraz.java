import java.util.Random;


class Obraz {

    private int size_n;
    private int size_m;
    private char[][] tab;
    private char[] tab_symb;
    private int[] histogram;
    private int[] hist_parallel;   // NOWE: histogram liczony przez wątki

    // ZMIANA: trzeci parametr - z ilu pierwszych symboli losujemy znaki obrazu
    public Obraz(int n, int m, int liczba_znakow) {

        this.size_n = n;
        this.size_m = m;
        tab = new char[n][m];
        tab_symb = new char[94];

        final Random random = new Random();

        // for general case where symbols could be not just integers
        for(int k=0;k<94;k++) {
            tab_symb[k] = (char)(k+33); // substitute symbols
        }

        for(int i=0;i<n;i++) {
            for(int j=0;j<m;j++) {
                // ZMIANA: losujemy tylko spośród liczba_znakow pierwszych symboli
                tab[i][j] = tab_symb[random.nextInt(liczba_znakow)];
                System.out.print(tab[i][j]+" ");
            }
            System.out.print("\n");
        }
        System.out.print("\n\n");

        histogram = new int[94];
        clear_histogram();

        hist_parallel = new int[94];   // NOWE: Java wypełnia nową tablicę zerami
    }

    public void clear_histogram(){

        for(int i=0;i<94;i++) histogram[i]=0;

    }

    public void calculate_histogram(){

        for(int i=0;i<size_n;i++) {
            for(int j=0;j<size_m;j++) {

                // optymalna wersja obliczania histogramu, wykorzystująca fakt, że symbole w tablicy
                // można przekształcić w indeksy tablicy histogramu
                // histogram[(int)tab[i][j]-33]++;

                // wersja bardziej ogólna dla tablicy symboli nie utożsamianych z indeksami
                // tylko dla tej wersji sensowne jest zrównoleglenie w dziedzinie zbioru znaków ASCII
                for(int k=0;k<94;k++) {
                    if(tab[i][j] == tab_symb[k]) histogram[k]++;
                    //if(tab[i][j] == (char)(k+33)) histogram[k]++;
                }

            }
        }

    }

    public void print_histogram(){

        for(int i=0;i<94;i++) {
            System.out.print(tab_symb[i]+" "+histogram[i]+"\n");
            //System.out.print((char)(i+33)+" "+histogram[i]+"\n");
        }

    }

    // ===================== NOWE METODY: wariant 1 =====================

    // Liczy wystąpienia JEDNEGO symbolu tab_symb[k] w całym obrazie.
    // Wynik trafia do hist_parallel[k]. Każdy wątek dostaje inne k,
    // więc żadne dwa wątki nie piszą do tej samej komórki - nie trzeba synchronized.
    public void calculate_histogram_znak(int k) {
        for (int i = 0; i < size_n; i++) {
            for (int j = 0; j < size_m; j++) {
                if (tab[i][j] == tab_symb[k]) hist_parallel[k]++;
            }
        }
    }

    // Drukuje jeden wiersz w postaci "Wątek 3: $ =====".
    // synchronized: w danej chwili tę metodę może wykonywać tylko jeden wątek,
    // więc wiersze różnych wątków nie mieszają się ze sobą.
    public synchronized void print_histogram_znak(int nr_watku, int k) {
        System.out.print("Wątek " + nr_watku + ": " + tab_symb[k] + " ");
        for (int i = 0; i < hist_parallel[k]; i++) {
            System.out.print("=");          // jeden znak "=" na jedno wystąpienie
        }
        System.out.print("\n");
    }

    // ===================== NOWE METODY: wariant 2 =====================

    // Liczy wystąpienia symboli o indeksach start_znak, ..., koniec_znak-1 (blok symboli).
    // To "uniwersalny wzorzec" z komentarza powyżej: wszystkie wiersze, wszystkie kolumny,
    // ale tylko symbole z bloku. Bloki różnych wątków są rozłączne,
    // więc wątki piszą do różnych komórek hist_parallel - nie trzeba synchronized.
    public void calculate_histogram_zakres(int start_znak, int koniec_znak) {
        for (int i = 0; i < size_n; i++) {
            for (int j = 0; j < size_m; j++) {
                for (int k = start_znak; k < koniec_znak; k++) {
                    if (tab[i][j] == tab_symb[k]) hist_parallel[k]++;
                }
            }
        }
    }

    // ===================== NOWE METODY: wariant 3 =====================

    // Liczy histogram LOKALNY dla wierszy nr, nr+liczba_watkow, nr+2*liczba_watkow, ...
    // (podział cykliczny wierszowy). Wątek liczy wszystkie symbole, więc można użyć
    // wersji optymalnej: indeks w histogramie = kod znaku - 33.
    // Każde wywołanie tworzy własną tablicę lokalny, więc nie trzeba synchronized.
    public int[] calculate_histogram_wiersze_cyklicznie(int nr, int liczba_watkow) {
        int[] lokalny = new int[94];                          // własny histogram wątku (same zera)
        for (int i = nr; i < size_n; i += liczba_watkow) {    // co liczba_watkow-ty wiersz
            for (int j = 0; j < size_m; j++) {                // wszystkie kolumny wiersza
                lokalny[(int)tab[i][j] - 33]++;               // wersja optymalna
            }
        }
        return lokalny;                                       // oddajemy wynik wywołującemu
    }

    // Dodaje histogram lokalny wątku do wspólnego hist_parallel (i wypisuje go dla kontroli).
    // synchronized: wszystkie wątki zmieniają TE SAME komórki hist_parallel,
    // więc dodawanie musi odbywać się po kolei (inaczej wyścig i zgubione dodawania).
    public synchronized void dodaj_histogram_lokalny(int nr_watku, int[] lokalny) {
        System.out.print("Wątek " + nr_watku + " (lokalnie):");
        for (int k = 0; k < 94; k++) {
            if (lokalny[k] > 0) System.out.print(" " + tab_symb[k] + ":" + lokalny[k]);
            hist_parallel[k] += lokalny[k];                   // sumowanie wyników częściowych
        }
        System.out.print("\n");
    }

    // Drukuje hist_parallel w tym samym formacie co print_histogram.
    // Wywołuje ją main po join(), więc nie trzeba synchronized.
    public void print_hist_parallel() {
        for (int k = 0; k < 94; k++) {
            System.out.print(tab_symb[k] + " " + hist_parallel[k] + "\n");
        }
    }

    // ===================== NOWE METODY: wariant 4 =====================

    // Liczy histogram LOKALNY dla kolumn start_kol, ..., koniec_kol-1 (blok kolumn),
    // we wszystkich wierszach. Wątek liczy wszystkie symbole - wersja optymalna.
    // Każde wywołanie tworzy własną tablicę lokalny, więc nie trzeba synchronized.
    // Wynik dodaje się do hist_parallel tą samą metodą co w wariancie 3 (dodaj_histogram_lokalny).
    public int[] calculate_histogram_kolumny_blokowo(int start_kol, int koniec_kol) {
        int[] lokalny = new int[94];                          // własny histogram wątku (same zera)
        for (int i = 0; i < size_n; i++) {                    // wszystkie wiersze
            for (int j = start_kol; j < koniec_kol; j++) {    // tylko kolumny z bloku wątku
                lokalny[(int)tab[i][j] - 33]++;               // wersja optymalna
            }
        }
        return lokalny;
    }

    // Porównuje histogram sekwencyjny (histogram) z równoległym (hist_parallel).
    public void compare_histograms() {
        boolean zgodne = true;
        for (int k = 0; k < 94; k++) {
            if (histogram[k] != hist_parallel[k]) {
                System.out.println("Różnica dla symbolu " + tab_symb[k]
                        + ": sekwencyjnie " + histogram[k]
                        + ", równolegle " + hist_parallel[k]);
                zgodne = false;
            }
        }
        if (zgodne) System.out.println("Histogramy zgodne");
        else        System.out.println("BŁĄD: histogramy różne");
    }

}
