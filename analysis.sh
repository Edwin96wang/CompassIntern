#! /bin/bash

run_processor(){
  root_name="~/analysis_compass/hist/hist${run_number[$run_num]}.root"
  arg_phast="-u 120 ${run_arg[$run_num]}"
  taskset -c $pNum ./phast -h $root_name $arg_phast > /dev/null
}

function check_exist(){
    num=$1
    shift
    t=( $@ )
    for (( i_check_exist=0; i_check_exist<${#t[@]}; i_check_exist++ ))
    do
        if [ ${t[$i_check_exist]} == $num ]
        then
            return 0
            echo "got it"
        fi
    done
    return 1
}


#select data files and run number arguments
datapath=/mnt/data/compass/2008
working_path="~/analysis_compass"
rm ~/analysis_compass/hist/*.root
array=( $( ls $datapath/2008preselectionW3*/mDST-*.full ) )
echo "the number of files: ${#array[@]}"
filenum=${#array[@]}

run=0
run_last=0
j=0
run_number[0]=${array[0]:48:5}
for (( i=0; i<$filenum; i++))
do
  run=${array[$i]:48:5}

  if [ $run -eq $run_last ] || [ $run_last -eq 0 ]
  then
    run_arg[j]="${run_arg[j]}${array[$i]} "
  else
    ((j++))
    run_number[j]=${array[$i]:48:5}
    run_arg[j]="${array[$i]} "
  fi

  run_last=$run
done



#pick out the corrupted data files:
runnumber_delete=( "69854" "70183" "70188" "70225" )
for (( i=0; i<${#run_number[@]}; i++ ))
do
  if check_exist "${run_number[$i]}" "${runnumber_delete[@]}"
  then
    echo -e "${run_number[$i]} is deleted"
    unset run_arg[$i]
    unset run_number[$i]
  fi
done
run_arg=("${run_arg[@]}")
run_number=("${run_number[@]}")

echo "the number of valid RunTimes: ${#run_number[@]}"


#execute the data files
cd $TOOLS
make > /dev/null
if [ $? -eq 0 ]
then
  echo "make process successful"

  for (( i=0; i<4; i++ ))
  do
    P[i]=111111
  done

  if  ps -p ${P[0]} > /dev/null || ps -p ${P[1]} > /dev/null || ps -p ${P[2]} > /dev/null || ps -p ${P[3]} > /dev/null
  then
    echo -e "warning: processors have been occupied, please reinitialize the PIDs\n"
  else
    echo -e "PID initialization successful\n"
  fi

  run_num=0
  while [ $run_num -lt ${#run_arg[@]} ]
  do
    for (( pNum=0; pNum<4; pNum++ ))
    do
      if ! ps -p ${P[$pNum]} > /dev/null
      then
        if ! [ $run_num -lt ${#run_arg[@]}  ]
        then
          break
        else
          run_processor &
          P[$pNum]=$!
          echo -e "$run_num: run number ${run_number[$run_num]} is being analyzed at kernel $pNum\n"
          (( run_num++ ))
        fi
      fi
    done
    sleep 0.2
  done

  wait ${P[0]} ${P[1]} ${P[2]} ${P[3]}
  echo "****************"
  echo "data files analysis is ended"
  echo "****************"

  echo "starting ROOT...."
  hist_file="$working_path/hist_analyse.cxx"
  root -l -q $hist_file > /dev/null
  echo "****************"
  echo "ROOT analysis is ended"
  echo "****************"
else
  echo "error occurs during make"
fi
