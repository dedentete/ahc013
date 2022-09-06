#!/bin/bash
g++ -o main.out ../main.cpp
./main.out < in/0001.txt > out/0001.txt
cargo run --release --bin vis in/0001.txt out/0001.txt