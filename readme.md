cd src
g++ -std=c++11 main.cpp -o main -fopenmp -O3

copy main.exe into folder instances

cd instances
./main.exe instance01 -t 10