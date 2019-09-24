#! /bin/bash

function check_exist(){
    num=$1
    shift
    t=( $@ )
    for (( i=0; i<${#t[@]}; i++ ))
    do
        if [ ${t[$i]} == $num ]
        then
            return 0
            echo "got it"
        fi
    done
    return 1
}



array=("0020" "4520" "4589" "4567" "12345" "5879" "56324")

if check_exist "36" "${array[@]}"
then
    echo "true"
else
    echo "false"
fi