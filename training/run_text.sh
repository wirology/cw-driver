#!/bin/bash
if [ -z $1 ] ; then
name=`basename $0`
  echo "usage: $name <training text> [word speed] [character speed]"
  exit 1
fi
text=$1
wspeed=$2
cspeed=$3
if [ -z $wspeed ] ; then
  wspeed=24
fi

if [ -z $cspeed ] ; then
  cspeed=`expr $wspeed + 4`
fi 

sdir=`dirname $0`
cat $text | $sdir/filter.sh | $sdir/run_ascii2pcm_training.sh $wspeed $cspeed
