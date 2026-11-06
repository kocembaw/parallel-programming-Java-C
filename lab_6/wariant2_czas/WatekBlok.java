// Wariant 2 (wersja do pomiaru czasu): zadanie dla wątku jako klasa implementująca Runnable
class WatekBlok implements Runnable {

    private int nr;            // numer wątku
    private int start_znak;    // indeks pierwszego symbolu bloku
    private int koniec_znak;   // indeks ZA ostatnim symbolem bloku
    private Obraz obraz;       // wspólny obraz

    public WatekBlok(int nr, int start_znak, int koniec_znak, Obraz obraz) {
        this.nr = nr;
        this.start_znak = start_znak;
        this.koniec_znak = koniec_znak;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        obraz.calculate_histogram_zakres(start_znak, koniec_znak);   // policz cały swój blok
        // POMIAR CZASU: bez wydruku - drukowanie trwałoby dłużej niż liczenie
    }
}
