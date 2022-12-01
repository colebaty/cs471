#!/usr/bin/bash

# automatically compile program and run for given sample values in $p and $f
# data is stored in file 'samples'

p=(512 1024 2048)
f=(4 8 12)

gen_stats () {
    echo "exporting sample data to stats.csv"

    echo "p,f,num pgs,alg,fault ratio" > stats.csv

    for i in ${p[@]}; do 
        for j in ${f[@]}; do 
            end=$(cat sample-$i-$j | grep -vi "page size" | awk '{ printf "%s,%s,%s\n", $2,$3,$4 }')
            for n in $end; do
                echo "$i,$j,$n" >> stats.csv # page size, num frames
            done;
        done; 
    done ;
}

if [[ ! -f sample* ]]; then
    echo "no samples found; generating samples"
    ./run.sh
    gen_stats
else
    gen_stats
fi

cat stats.csv
