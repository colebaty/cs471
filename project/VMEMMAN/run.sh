 #!/usr/bin/bash
 
p=(512 1024 2048)
f=(4 8 12)

 for i in ${p[@]}; do 
    for j in ${f[@]}; do 
        echo "=====| p = $i | f = $j |=====" >> samples
        ./bin/main $i $j input | grep numrefs >> samples
        echo "=============================" >> samples
        echo >> samples; 
    done; 
done 