#!/bin/bash
if [[ $# -eq 2 ]]; then
    declare -a Instances=( "instance02" "instance03" "instance04" "instance05" "instance06" "instance07" "instance08" )

    ## UNCOMMENT TO CLEAR LOGS AT EVERY RUN
    if ls ./logs/log_* 1> /dev/null 2>&1;then
        rm logs/log_*
    fi
    if ls ./logs/best/log_* 1> /dev/null 2>&1;then
        rm logs/best/log_*
    fi

    for instance in ${Instances[@]}; do
        ./gridsearch.sh $instance $1 $2
        # sleep 56700
        sleep 1
    done
else
    echo "Use as:"
    echo "./test.sh seconds num_parallel_instances (cores/4)"
fi
