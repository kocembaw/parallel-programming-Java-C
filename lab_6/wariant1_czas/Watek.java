// Wariant 1 (wersja do pomiaru czasu): wątek jako klasa dziedzicząca po Thread
class Watek extends Thread {

    private int nr;        // numer wątku = indeks symbolu, za który wątek odpowiada
    private Obraz obraz;   // wspólny obraz - wszystkie wątki dostają ten sam obiekt

    public Watek(int nr, Obraz obraz) {
        this.nr = nr;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        obraz.calculate_histogram_znak(nr);     // policz swój symbol
        // POMIAR CZASU: bez wydruku - drukowanie trwałoby dłużej niż liczenie
    }
}
