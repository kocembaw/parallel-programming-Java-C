// Wariant 4: podział blokowy kolumnowy (Runnable).
// Wątek dostaje ciągły blok kolumn start_kol, ..., koniec_kol-1.
class WatekKolumny implements Runnable {

    private int nr;           // numer wątku (do wydruku)
    private int start_kol;    // pierwsza kolumna bloku
    private int koniec_kol;   // kolumna ZA ostatnią kolumną bloku
    private Obraz obraz;      // wspólny obraz

    public WatekKolumny(int nr, int start_kol, int koniec_kol, Obraz obraz) {
        this.nr = nr;
        this.start_kol = start_kol;
        this.koniec_kol = koniec_kol;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        // 1) policz histogram swojego bloku kolumn - bez synchronizacji, własna tablica
        int[] lokalny = obraz.calculate_histogram_kolumny_blokowo(start_kol, koniec_kol);
        // 2) dodaj go do wspólnego wyniku - metoda synchronized z wariantu 3
        obraz.dodaj_histogram_lokalny(nr, lokalny);
    }
}
