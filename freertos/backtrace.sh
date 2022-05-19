#!/bin/sh

log=$1
elf_file=$2

ADDR2LINE=xt-addr2line

echo ""

for line in $(cat $log)
do
	line=`echo $line | tr -d "\n"`
	line=`echo $line | tr -d "\r"`
	if [ -z $line ];then
		continue;
	fi
	result=`$ADDR2LINE -f -e $elf_file -a $line`
	echo $result
	echo ""
done
