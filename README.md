Per compilare usare
```
clang++ -std=c++11 -stdlib=libc++ -Weverything main.cpp -o main
```
per fare build e creare main.exe, lancia **f6** da *VsCode*

per lanciare il programma, devi aprire il terminale ed eseguire
```
cd src
./main.exe instance01
```

per usare gridsearch.sh
```
./gridsearch.sh instance0# int[time]
e.g. ./gridsearch.sh instance01 180
```
il log dell'istanza migliore viene salvato in logs/best