#!/bin/sh

cp -f /mnt/nandflash/4G/lte-connect-script /etc/ppp/peers/
cp -f /mnt/nandflash/4G/chat-lte-script /etc/ppp/
cp -f /mnt/nandflash/4G/libutil-2.10.1.so /lib/
rm -rf /lib/libutil.so.1
ln -s /lib/libutil-2.10.1.so /lib/libutil.so.1
cp -f /mnt/nandflash/4G/pptp /usr/sbin/
chmod +x /usr/sbin/pptp
cp -f /mnt/nandflash/4G/chap-secrets /etc/ppp/
cp -f /mnt/nandflash/4G/options.pptp /etc/ppp/
cp -f /mnt/nandflash/4G/pptpvpn /etc/ppp/peers/
