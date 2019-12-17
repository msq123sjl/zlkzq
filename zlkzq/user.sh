#!/bin/sh

if test -e /mnt/nandflash/bin/libcommon.so
then
    mv /mnt/nandflash/bin/libcommon.so /lib/
fi 

export set QTDIR=/opt/qt-4.8 
export set QPEDIR=/opt/qt-4.8 
export set QWS_DISPLAY='LinuxFB:/dev/fb0' 
export set QWS_DISPLAY='LinuxFB:mmWidth130:mmHeight100:0' 
export set QWS_KEYBOARD='TTY:/dev/tty1' 

export set TSLIB_TSDEVICE=/dev/input/event0 
export set TSLIB_CALIBFILE=/etc/pointercal 
export set TSLIB_CONFFILE=/etc/ts.conf 
export set TSLIB_PLUGINDIR=/lib/ts 
export set QWS_MOUSE_PROTO='TSLIB:/dev/input/event0' 
        
export set QT_PLUGIN_PATH=$QTDIR/plugins/ 
export set QT_QWS_FONTDIR=$QTDIR/lib/fonts/ 
export set PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin:/mnt/nandflash/:/mnt/sdcard/ 
export set LD_LIBRARY_PATH=$QTDIR/lib:$QPEDIR/plugins/imageformats:$LD_LIBRARY_PATH 

#定时任务
CRABTAB=/var/spool/cron/crontabs
if [ ! -e $CRABTAB ]
then
    mkdir -p $CRABTAB
fi
CRON="*/5 * * * * /mnt/nandflash/ping.sh"
echo "$CRON" > $CRABTAB/root

echo 0 > /sys/class/backlight/backlight/brightness

cd /mnt/nandflash/bin

httpd -c httpd.conf
#./readmeter&
#./interface -qws&
./watchdog&
#./autodial
#nohup "/mnt/nandflash/bin/watchdog"  >> /mnt/sdcard/mythread.log 2>&1 &
