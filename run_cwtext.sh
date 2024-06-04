#!/bin/sh
sdir=`dirname $0`
speed=$1
ffactor=$2
if [ -z "$speed" ] ; then
	speed=24
fi

#speed=`expr $speed / 2`

if [ -z "$ffactor" ] ; then
	fspeed=$speed
else
	fadd=`expr $speed / $ffactor`
	fspeed=`expr $speed + $fadd`
fi
echo $fspeed

$sdir/cwtext-0.96/cwpcm -w $speed -F $fspeed -v 50 -f 800  | play -t raw -r 44100 -e unsigned -b 8 -c 1 - bandpass -c 800 50
