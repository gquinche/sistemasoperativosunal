for size in 1 10 100 1000 
do
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