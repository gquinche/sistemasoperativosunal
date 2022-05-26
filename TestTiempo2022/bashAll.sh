
for strat in pipes shmem fileShare
do
    for size in 1 10 100 1000 10000 100000 1000000 10000000 100000000
    do  echo ""
        echo " $strat with size $size bytes"
        echo ""
        if [ $size -lt 101 ] 
        then 
            hyperfine "./$strat $size" --runs 10  
        fi
        if [ $size -gt 100 ]
        then
            hyperfine "./$strat $size" --runs 1
        fi
        
    done
done 