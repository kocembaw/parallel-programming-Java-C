import  java.util.Random; // udostepnia klase do losowania liczb


class Obraz {
    // 5 zmiennych ktore ma kazdy obiekt Obraz
    private int size_n; // liczba wierszy
    private int size_m; // liczba kolumn
    private char[][] tab; // obraz
    private char[] tab_symb; // 94 mozliwe symbole
    private int[] histogram; // 94 liczniki, histogram[k] = liczba wystąpień tab_symb[k]
    
    public Obraz(int n, int m) {
	// zapamietuje wymiary
	this.size_n = n;
	this.size_m = m;
	tab = new char[n][m];
	tab_symb = new char[94];
	
	final Random random = new Random();
	
	// for general case where symbols could be not just integers
	for(int k=0;k<94;k++) {
	    tab_symb[k] = (char)(k+33); // Wypełnia tab_symb znakami
	}

	for(int i=0;i<n;i++) { // przechodzi po wierszach
	    for(int j=0;j<m;j++) {	//przechodzi po kolumnach

		tab[i][j] = tab_symb[random.nextInt(94)];  // zapisuje symbol w biezacej komorce obrazu 
		//tab[i][j] = (char)(random.nextInt(94)+33);  // ascii 33-127
		System.out.print(tab[i][j]+" "); // wypisuje znak komorki i spację
	    }
	    System.out.print("\n"); // po całym wierszu, program przechodzi do nowej linii
	}
	System.out.print("\n\n"); 
	
	histogram = new int[94];
   	clear_histogram();
    }
    
    public void clear_histogram(){

	for(int i=0;i<94;i++) histogram[i]=0;

    }

// Funkcja liczy, ile razy każdy z 94 symboli występuje w obrazie, i zapisuje wyniki w tablicy histogram
    public void calculate_histogram(){
 // Tutaj jest size_n zamiast n, bo n i m były parametrami konstruktora i istniały tylko w czasie jego działania.
	  for(int i=0;i<size_n;i++) {
	    for(int j=0;j<size_m;j++) {

		// optymalna wersja obliczania histogramu, wykorzystująca fakt, że symbole w tablicy
		// można przekształcić w indeksy tablicy histogramu
		// histogram[(int)tab[i][j]-33]++;
		
		// wersja bardziej ogólna dla tablicy symboli nie utożsamianych z indeksami
		// tylko dla tej wersji sensowne jest zrównoleglenie w dziedzinie zbioru znaków ASCII
		  for(int k=0;k<94;k++) {
		    if(tab[i][j] == tab_symb[k]) histogram[k]++; //Dla bieżącej komórki funkcja sprawdza po kolei wszystkie 94 symbole. == poró
                                                                 //wnuje dwa znaki czyli ich kody
		    //if(tab[i][j] == (char)(k+33)) histogram[k]++;	    
		  }

	    }
	  }

    }

// uniwersalny wzorzec dla różnych wariantów zrównoleglenia - można go modyfikować dla
// różnych wersji dekompozycji albo stosować tak jak jest zapisane poniżej zmieniając tylko
// parametry wywołania w wątkach
//
//calculate_histogram_wzorzec(start_wiersz, end_wiersz, skok_wiersz,
//                            start_kol, end_kol, skok_kol,
//                            start_znak, end_znak, skok_znak){
//
//  for(int i=start_wiersz;i<end_wiersz;i+=skok_wiersz) {
//     for(int j=start_kol;j<end_kol;j+=skok_kol) {
//        for(int k=start_znak;k<end_znak;k+=skok_znak) {
//           if(tab[i][j] == tab_symb[k]) histogram[k]++;
//


    public void print_histogram(){
	
	for(int i=0;i<94;i++) {
	    System.out.print(tab_symb[i]+" "+histogram[i]+"\n");	    
	    //System.out.print((char)(i+33)+" "+histogram[i]+"\n");	    
	}

    }

}
