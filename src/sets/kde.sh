cp kdm /etc/conf.d/xdm
if [ $1 -eq "rc" ]
then
	rc-update add xdm default
elif [ $1 -eq "systemd" ]
	systemctl enable kdm
