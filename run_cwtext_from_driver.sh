#!/bin/sh
# may need to replace unsigned by signed in play args
sdir=`dirname $0`
speed=$1
fspeed=$2
sidetone=8000
bandwith=`expr 300 \* $fspeed / 10`
$sdir/cwtext-0.96/cwpcm -w $speed -F $fspeed -v 70 -f $sidetone  | play -t raw -r 44100 -e unsigned -b 8 -c 1 - bandpass -c $sidetone $bandwith > /dev/null 2>&1
