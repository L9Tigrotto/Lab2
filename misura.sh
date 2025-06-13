#!/bin/bash

clients_temp_output="output/clients_temp.txt"
clients_support_output="output/clients_support.txt"
supervisor_output="output/supervisor_only.txt"
table_output="output/table_only.txt"

rm -f $clients_temp_output
rm -f $clients_support_output
rm -f $supervisor_output
rm -f $table_output

grep "SECRET" $2 >> $clients_temp_output
grep "SUPERVISOR ESTIMATE" $1 >> $supervisor_output
grep "BASED ON" $supervisor_output >> $table_output

while read line
do
    columns=( $line )

    echo "$((16#${columns[1]})) ${columns[3]}" >> $clients_support_output
done < $clients_temp_output

readed=0
correct=0
sum=0
while read line
do
    columns=( $line )
    res=( $( grep "${columns[4]}" $clients_support_output ) )

    estimated=${columns[2]}
    real=${res[1]}
    
    distance=$(( $estimated - $real ))
    distance=${distance#-}

    if [ $distance -le 25 ]
    then
        correct=$(( correct + 1 ))
    fi

    readed=$(( readed + 1 ))
    sum=$(( sum + distance ))
done < $table_output

error=$( echo "scale=3; $sum / $readed" | bc )

echo "correct: $correct"
echo "average error: $error"
