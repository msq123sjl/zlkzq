# 参数1 服务器IP
# 参数2 隧道IP
# 参数3 VPN用户名
#!/bin/sh
#echo -e "#!/bin/sh\n/bin/cp -rf ./${NAME}/ /usr/local/" > ./install
echo -e "$3 $3 Sky123456 $2" > /etc/ppp/chap-secrets
sed -i "s/pty.*/pty \"pptp $1 --nolaunchpppd\"/g" /etc/ppp/peers/pptpvpn
sed -i "s/name.*/name $3/g" /etc/ppp/peers/pptpvpn
sed -i "s/remotename.*/remotename $3/g" /etc/ppp/peers/pptpvpn