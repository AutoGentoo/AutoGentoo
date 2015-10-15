ESC_SEQ="\x1b["
COL_RESET=$ESC_SEQ"39;49;00m"
COL_RED=$ESC_SEQ"31;01m"
COL_GREEN=$ESC_SEQ"32;01m"
COL_YELLOW=$ESC_SEQ"33;01m"
COL_BLUE=$ESC_SEQ"34;01m"
COL_MAGENTA=$ESC_SEQ"35;01m"
COL_CYAN=$ESC_SEQ"36;01m"
cr="chroot /mnt/gentoo/ /bin/bash -c"
$cr 'emerge -qUDN @world' >> AutoGentoo.log
check=$($cr "echo $?")
if [ '$(check)' != '0' ]; then
	echo -ne "$COL_GREEN *$COL_RESET Masking Packages..."
	###WRITE IF FOR DESELECT
	getsys=$(cat sysd)
	if [ "$getsys" -eq "1" ]; then
		$cr 'emerge --deselect sys-apps/systemd' >> AutoGentoo.log
	else
		$cr 'emerge --deselect sys-fs/udev' >> AutoGentoo.log
	fi
	echo DONE
	echo -ne "$COL_GREEN *$COL_RESET Emerging pre-update packages..."
	$cr 'emerge -q audit' >> AutoGentoo.log
	$cr 'emerge -q libseccomp' >> AutoGentoo.log
	echo DONE
	echo -ne "$COL_GREEN *$COL_RESET Trying to update system..."
	$cr 'emerge -qUDN @world' >> AutoGentoo.log
	echo DONE
	check2=$($cr "echo $?")
	$cr 'emerge -q grub'
fi
