#!/bin/bash
n=$(($1-1))
sum=0
for i in `seq 0 $n`
do
    t=$(printf "%04d" $i)
    IFS=$' '
    file=(`cat score/$t.txt`)
    sum=`expr $sum + $((${file[2]}))`
done
echo $sum