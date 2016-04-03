#!/bin/sh
ln -sf /proc/self/mounts /etc/mtab
systemctl enable dhcpcd.service
if [ -d /etc/portage/package.use ]
then
	cp gnome /etc/portage/package.use/
else
	cp gnome /etc/portage/package.use/gnome
