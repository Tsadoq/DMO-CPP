#!/bin/bash
if [[ $# -eq 2 ]]; then
    if [[ ! -d logs ]];then
        mkdir logs
    fi
    # if ls ./logs/log_* 1> /dev/null 2>&1;then
    #     rm logs/log_*
    # fi
    # declare -a Alpha=( "0.5" "0.6" "0.7" "0.8" )
    # declare -a Cooling=( "0.8" "0.85" "0.9" )
    # declare -a Mutations=( "2" "3" "4" )
    echo "Insert values for alpha: "
    read -r alpha

    echo "Insert values for cooling coefficient: "
    read -r cooling

    echo "Insert values for number of mutations: "
    read -r mutations

    let i=0
    echo "Testing on instance $1"
    for a in $alpha; do
        for cool in $cooling; do
            for mut in $mutations; do
                if [ $i -eq 2 ];then
                    let i=0
                    sleep $2
                fi
                today=$(date '+%Y%m%d_%H:%M:%S')
                filename="log_${today}_$1_alpha${a}_cool${cool}_mut${mut}.txt"
                ./main $1 $2 4 $a $mut $cool  > ./logs/$filename &
                echo "Running with alpha: $a, cool: $cool, mut: $mut"
                let i++
            done
        done
    done
else
    echo "Usage: ./gridsearch.sh instance_name runtime"
    echo "Example: ./gridsearch.sh instance01 180"
fi