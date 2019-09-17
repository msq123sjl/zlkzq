#!/bin/sh
MYPATH=$(cd `dirname $0`; pwd)
cd ${MYPATH}

#test -f ./nandflash.tar.gz && rm ./nandflash.tar.gz
#tar cvzf nandflash.tar.gz /mnt/nandflash/

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

test -d ./bin || mkdir -p ./bin 
test -d ./4G || mkdir -p ./4G
test -d ./para || mkdir -p ./para
test -d ./msg || mkdir -p ./msg
test -d ./shm || mkdir -p ./shm

test -f ./${NAME}/*.sh && /bin/mv ./${NAME}/*.sh /mnt/nandflash/
test -f ./${NAME}/userinfo.txt && /bin/mv ./${NAME}/userinfo.txt /mnt/nandflash/
/bin/mv ./${NAME}/bin/*.so /lib/
/bin/mv ./${NAME}/bin/* /mnt/nandflash/bin/
/bin/mv ./${NAME}/para/* /mnt/nandflash/para/
/bin/mv ./${NAME}/4G/* /mnt/nandflash/4G/

chmod -R +x /mnt/nandflash/
/mnt/nandflash/4G/4G_install.sh
rm -rf ./${NAME}
reboot
