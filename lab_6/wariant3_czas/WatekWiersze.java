// Wariant 3: podział cykliczny wierszowy (Runnable).
// Wątek nr dostaje wiersze nr, nr+liczba_watkow, nr+2*liczba_watkow, ...
class WatekWiersze implements Runnable {

    private int nr;              // numer wątku = pierwszy wiersz wątku
    private int liczba_watkow;   // skok między kolejnymi wierszami wątku
    private Obraz obraz;         // wspólny obraz

    public WatekWiersze(int nr, int liczba_watkow, Obraz obraz) {
        this.nr = nr;
        this.liczba_watkow = liczba_watkow;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        // 1) policz histogram swoich wierszy - bez synchronizacji, każdy wątek ma własną tablicę
        int[] lokalny = obraz.calculate_histogram_wiersze_cyklicznie(nr, liczba_watkow);
        // 2) dodaj go do wspólnego wyniku - metoda synchronized, wątki robią to po kolei
        obraz.dodaj_histogram_lokalny(nr, lokalny);
    }
}
