#!/bin/sh

#sn=`ping -c 1 -W 3 114.114.114.114 | grep "1 packets received" |awk '{print $4}'`

iLoop=0
while [ $iLoop -le 5 ]
do
    sn=`ping -c 1 -W 3 114.114.114.114 | grep "1 packets received" |awk '{print $4}'`
    if [ -z $sn ]
    then
        if [ $iLoop == 5 ]
        then
            AUTODIAL=`ps -w | grep autodial | grep -v grep |awk '{print $1}'`
            if [ -z $AUTODIAL ]
            then
                reboot
            else
                kill -13 $AUTODIAL > /dev/null
            fi
        fi
    else
        break
    fi
    sleep 3
    let "iLoop++"
done



