 #!/usr/bin/bash

 # automatically compile program and run for given sample values in $p and $f
 # data is stored in file 'samples'
 
p=(512 1024 2048)
f=(4 8 12)

if [ ! -f main ]; then
    echo "executable not found; compiling"
    g++ -fpermissive -std=c++17 -g main.cpp -o main
fi

if [ ! -f input ]; then
    echo "no input file found"
    echo "please place input file in same directory as executable"
    echo "file must have name 'input'"
else
    echo "running simulations; data stored in file 'samples'"
    echo > samples

    for i in ${p[@]}; do 
        for j in ${f[@]}; do 
            echo "=====| p = $i | f = $j |=====" >> samples
            ./main $i $j input >> samples
            echo "=============================" >> samples
            echo >> samples; 
        done; 
    done 
    cat samples
fi