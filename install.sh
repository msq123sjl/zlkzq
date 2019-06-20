#!/bin/sh
MYPATH=$(cd `dirname $0`; pwd)
cd ${MYPATH}

test -f ./bin.tar.gz && rm ./bin.tar.gz
tar -zcvf bin.tar.gz ../bin/

NAME=zlkzq

LIST=" \
	watchdog interface dataproc up_main ValveControl\
"
kill_target(){
    echo "$1"
    sn=`ps -w | grep $1 | grep -v grep |awk '{print $1}'`
    if [ ! -z $sn ]
    then
        if [ "$1"x = "watchdog"x ]
        then
            kill -13 $sn > /dev/null
            echo "kill -13 $sn $1"
        else
            kill $sn > /dev/null
            echo "kill $sn $1"
        fi
    fi
}

for target in $LIST
	do
		kill_target ${target}
	done
    
test -f ./${NAME}/bin/libcommon.so && /bin/mv ./${NAME}/bin/libcommon.so /lib/
/bin/mv ./${NAME}/bin/* /mnt/nandflash/bin/
/bin/mv ./${NAME}/para/* /mnt/nandflash/para/
rm -rf ./${NAME}
reboot
