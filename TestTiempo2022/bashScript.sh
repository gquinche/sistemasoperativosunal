for size in 1000 10000 100000 1000000 10000000 100000000
do  
        echo "" 
        echo " SOCKETS with size $size bytes"
        echo ""
    ./server $size & 
    if [ $size -lt 101 ] 
    then 
        hyperfine "./client $size" --runs 10 --prepare "sleep 60" 
    fi
    if [ $size -gt 100 ]
    then
        hyperfine "./client $size" --runs 1
    fi
    
done