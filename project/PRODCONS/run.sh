#!/usr/bin/bash

p=(2 5 10)
c=(2 5 10)
b=(2 10)

if [ ! -f prodcons ]; then
    echo "no executable found; compiling"
    g++ -g -o prodcons -fpermissive -fdiagnostics-color=always main.cpp -lpthread
fi

for i in ${p[@]}; do
    for j in ${c[@]}; do
        for k in ${b[@]}; do
            echo ==============
            echo -n "running program with p=$i c=$j b=$k..."
            ./prodcons $i $j $k > "sample-$i-$j-$k"
            echo "done"
            echo ==============
            echo
            sleep 1
        done
    done
done