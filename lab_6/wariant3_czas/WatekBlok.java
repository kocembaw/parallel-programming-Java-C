// Wariant 2: zadanie dla wątku jako klasa implementująca interfejs Runnable.
// Obiekt tej klasy NIE jest wątkiem - to "praca do wykonania", którą dostaje obiekt Thread.
class WatekBlok implements Runnable {

    private int nr;            // numer wątku (do wydruku)
    private int start_znak;    // indeks pierwszego symbolu bloku
    private int koniec_znak;   // indeks ZA ostatnim symbolem bloku (jak w pętli: k < koniec_znak)
    private Obraz obraz;       // wspólny obraz

    public WatekBlok(int nr, int start_znak, int koniec_znak, Obraz obraz) {
        this.nr = nr;
        this.start_znak = start_znak;
        this.koniec_znak = koniec_znak;
        this.obraz = obraz;
    }

    // Jedyna metoda wymagana przez interfejs Runnable
    @Override
    public void run() {
        obraz.calculate_histogram_zakres(start_znak, koniec_znak);   // policz cały swój blok
        for (int k = start_znak; k < koniec_znak; k++) {
            obraz.print_histogram_znak(nr, k);   // każdy wiersz drukowany w całości (synchronized)
        }
    }
}
