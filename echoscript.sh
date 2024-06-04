#!/bin/bash

speed=$1
ffactor=$2

if [ -z "$speed" ] ; then
	speed=24
        ffactor=5
fi

while [ -z "" ]
do
	read -r var1
	echo $var1 | ./run_cwtext.sh $speed $ffactor > /dev/null 2>&1
done
