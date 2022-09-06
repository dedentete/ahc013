#!/bin/bash
n=$(($1-1))
g++ -o main.out ../main.cpp
for i in `seq 0 $n`
do
    t=$(printf "%04d" $i)
    ./main.out < in/$t.txt > out/$t.txt
done
for i in `seq 0 $n`
do
    t=$(printf "%04d" $i)
    cargo run --release --bin vis in/$t.txt out/$t.txt > score/$t.txt
done
bash calc.sh $1