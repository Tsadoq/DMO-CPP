#!/bin/bash
if [[ $# -eq 2 ]]; then
    if [[ ! -d logs ]];then
        mkdir logs
    fi
    Files=()
    
    ## UNCOMMENT TO CLEAR LOGS AT EVERY RUN
    # if ls ./logs/log_* 1> /dev/null 2>&1;then
    #     rm logs/log_*
    # fi

    ## SET LIST OF PARAMETERS HERE
    # declare -a Alpha=( "0.5" "0.6" "0.7" "0.8" )
    # declare -a Cooling=( "0.8" "0.85" "0.9" )
    # declare -a Mutations=( "2" "3" "4" )
    ## ----------- UNCOMMENT PREVIOUS LINES TO SET PARAM HERE
    
    
    ## OR INSERT THEM VIA COMMAND LINE
    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Insert values for alpha: "
    read -r alpha

    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Insert values for cooling coefficient: "
    read -r cooling

    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Insert values for number of mutations: "
    read -r mutations

    let i=0
    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Testing on instance $1"
    ## ----------- UNCOMMENT PREVIOUS LINES TO SET PARAM VIA COMMAND LINE

    for a in $alpha; do  ## param set via command line
    # for a in ${Alpha[@]}; do  ## param set as array
        for cool in $cooling; do  ## param set via command line
        # for cool in ${Cooling[@]}; do  ## param set as array
            for mut in $mutations; do  ## param set via command line
            # for mut in ${Mutations[@]}; do  ## param set as array
                if [[ $i -eq 2 ]];then
                    let i=0
                    sleep $2
                fi
                today=$(date '+%Y%m%d_%H:%M:%S')
                filename="log_${today}_$1_alpha${a}_cool${cool}_mut${mut}.txt"
                Files+=($filename)
                ./main $1 $2 4 $a $mut $cool  > ./logs/$filename &
                echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Running with alpha: $a, cool: $cool, mut: $mut"
                let i++
            done
        done
    done
else
    echo "Usage: ./gridsearch.sh instance_name runtime"
    echo "Example: ./gridsearch.sh instance01 180"
fi
sleep $2
sleep 1
max_score=99999
for f in ${Files[@]}; do
    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- "
    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- File: $f"
    score=$(cat logs/$f | grep 'Best solution' | tr -s "\t" | cut -f 2)
    echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Score: $score, max score: $max_score"
    if (( $(echo "$score < $max_score" |bc -l) ));then
        max_score=$score
        best=$f
    fi

done
echo "[$(date '+%Y-%m-%d_%H:%M:%S')] -- Best file found: $best with score $max_score"
if [[ ! -d logs/best ]];then
        mkdir logs/best
    fi
mv logs/$best logs/best
