#!/bin/sh
MYPATH=$(cd `dirname $0`; pwd)
cd ${MYPATH}
cp /mnt/nandflash/para/fs_para.conf /mnt/nandflash/para/fs_para.conf.bak
cp /mnt/nandflash/para/fs_net_para.conf /mnt/nandflash/para/fs_net_para.conf.bak
/mnt/nandflash/bin/config_para export


test -e /mnt/nandflash/para/watchdog.conf || /bin/mv /mnt/nandflash/scy/para/watchdog.conf /mnt/nandflash/para/watchdog.conf
#test -f ./nandflash.tar.gz && rm ./nandflash.tar.gz
#tar cvzf nandflash.tar.gz /mnt/nandflash/

NAME=zlkzq

LIST=" \
	watchdog autodial interface dataproc up_main ValveControl\
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

/bin/mv ./${NAME}/*.sh /mnt/nandflash/
test -e ./userinfo.txt || /bin/mv ./${NAME}/userinfo.txt /mnt/nandflash/
/bin/mv ./${NAME}/bin/*.so /lib/
/bin/mv ./${NAME}/bin/* /mnt/nandflash/bin/
chmod -R +x /mnt/nandflash/
ARG=$1
rm -rf /mnt/nandflash/para/fs_para.dat
rm -rf /mnt/nandflash/shm/shm_para
#rm -rf /mnt/sdcard/scy.db
#rm -rf /mnt/sdcard/info.db
if [ ! -z $ARG ]
then
    if [[ $ARG = "init" ]]
    then    
        rm -rf /mnt/nandflash/para/fs_data.dat
        rm -rf /mnt/nandflash/shm/shm_data     
	rm -rf /mnt/nandflash/para/fs_valve_para.dat
        rm -rf /mnt/nandflash/shm/shm_valve_para
        /bin/mv ./${NAME}/para/* /mnt/nandflash/para/
        /bin/mv ./${NAME}/4G/* /mnt/nandflash/4G/
        /mnt/nandflash/4G/4G_install.sh
        echo "init"
    elif [[ $ARG = "initshm" ]]
    then
        rm -rf /mnt/nandflash/para/fs_data.dat
        rm -rf /mnt/nandflash/shm/shm_data
	rm -rf /mnt/nandflash/para/fs_valve_para.dat
        rm -rf /mnt/nandflash/shm/shm_valve_para
        /mnt/nandflash/bin/config_para import
        echo "initshm"
    else
        /mnt/nandflash/bin/config_para import
        echo "install $ARG"
    fi
else
    /mnt/nandflash/bin/config_para import
    echo "install"
fi

rm -rf ./${NAME}
reboot
