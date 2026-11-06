#!/bin/bash
# Skrypt pomiarowy do etapu 2.
# Dla każdej kombinacji: opcje kompilacji x rozmiar tablicy x liczba wątków
# kompiluje program od nowa, uruchamia go 3 razy i dopisuje wszystko do ../wyniki_lab5.txt
# Rozmiary tablicy podajemy przy uruchomieniu skryptu, np.: bash pomiary.sh 1000 1000000

WYNIKI=../wyniki_lab5.txt

for OPCJE in "-g -DDEBUG" "-O3 -fopenmp"; do      # dwie wersje kompilacji z Makefile
  for ROZMIAR in "$@"; do                         # rozmiary podane przy uruchomieniu skryptu
    for LW in 1 2 4; do                           # liczba wątków
      echo "===== ETAP 2: OPT=\"$OPCJE\", ROZMIAR=$ROZMIAR, LICZBA_W=$LW =====" | tee -a $WYNIKI
      make clean > /dev/null
      make OPT="$OPCJE -DROZMIAR=$ROZMIAR -DLICZBA_W=$LW" 2>&1 | tee -a $WYNIKI
      for PROBA in 1 2 3; do
        echo "--- uruchomienie $PROBA ---" | tee -a $WYNIKI
        ./pthreads_suma 2>&1 | tee -a $WYNIKI
      done
    done
  done
done
