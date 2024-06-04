#!/bin/bash

sv=$1
if [ -z "$sv" ] ; then
  sv=5
fi

count=10
while [ $count -gt 0 ] ; do
  count=`expr $count - 1`
  echo 'testword'
  sleep $sv
done

