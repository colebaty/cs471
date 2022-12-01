#!/usr/bin/bash

# automatically compile program and run for given sample values in $p and $f
# data is stored in file 'samples'
 
p=(512 1024 2048)
f=(4 8 12)

if [ -f main ]; then
    modsecs=$(date --reference=main +%s)
    nowsecs=$(date +%s)
    delta=$(($nowsecs - $modsecs))
    if [ $delta -gt 900 ]; then
        echo "executable too old; compiling"
        g++ -fpermissive -std=c++17 -g main.cpp -o main
    fi
else
    echo "executable not found; compiling"
    g++ -fpermissive -std=c++17 -g main.cpp -o main
fi

if [ ! -f input ]; then
    echo "no input file found"
    echo "please place input file in same directory as executable"
    echo "file must have name 'input'"
else
    echo "running simulations; data stored in file 'sample-p-f'"

    for i in ${p[@]}; do 
        for j in ${f[@]}; do 
            ./main $i $j input > "sample-$i-$j"
        done; 
    done 
fi