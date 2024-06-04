#!/bin/bash

speed=$1
ffactor=$2

MorseText() {
	morse=$1
	echo $morse | ./run_cwtext.sh $speed $ffactor > /dev/null 2>&1
}

MorseText2() {
	morse=$1
	echo $morse
	MorseText "$morse"
}

FuncF2() {
	MorseText2 'va2iu'
}


FuncF3() {
        text='cfm va2iu 1 qc tu'
	MorseText2 "${text}"
}

ReadKey() {
  if read -sN1 KEY; then
    while read -sN1 -t 0.001 ; do
      KEY+="${REPLY}"
    done
  fi
}

FuncDefault() {
  L1=$1
  if [ -z "$L1" ] ; then
    read -e res
  else
    read -i $L1 -e res
  fi
  MorseText "$res"  
}


if [ -z "$speed" ] ; then
	speed=24
        ffactor=5
fi


while ReadKey; do
  case "${KEY}" in
    #$'\eOP')   echo 'F1';;
    $'\eOQ')   FuncF2 ;;
    $'\eOR')   FuncF3 ;;
    #$'\e[24~') echo 'F12';;
    *) FuncDefault "${KEY}" ;;
  esac
done

