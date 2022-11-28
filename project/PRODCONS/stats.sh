#!/usr/bin/bash

p=(2 5 10)
c=(2 5 10)
b=(2 10)

echo "compiling statistics"

echo "p,c,b,dur" > stats.csv

for i in ${p[@]}; do
    for j in ${c[@]}; do
        for k in ${b[@]}; do
            if [ ! -f "sample-$i-$j-$k" ]; then
                echo "sample-$i-$j-$k not found; skipping"
                continue
            else
                dur=$(tail -n1 "sample-$i-$j-$k" | cut -d' ' -f4)
                echo "$i,$j,$k,$dur" >> stats.csv
            fi
        done
    done
done
