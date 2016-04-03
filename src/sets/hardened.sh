if [ $1 -eq "PIE" ]
then 
	SEARCH="hardenednossp"
elif [ $1 -eq "SSP" ]
then 
	SEARCH="hardenednopie"
else
	SEARCH="hardenednopiessp"
export SEARCH
GCC_CONFIG=$(ls /etc/env.d/gcc/ | grep -v config | grep -i $SEARCH)
gcc-config $GCC_CONFIG
emerge --depclean prelink
if [ -f /etc/portage/env/ && $1 -eq "PIE" ]
then
	cp nossp /etc/portage/env/nossp
else
	mkdir /etc/portage/env
	cp nossp /etc/portage/env/nossp
if [ -f /etc/portage/env/ && $1 -eq "SSP" ]
then
	cp nopie /etc/portage/env/nopie
else
	mkdir /etc/portage/env
	cp nopie /etc/portage/env/nopie
if [ $1 -eq "SSP" ]
then
	echo "sys-libs/zlib nopie" >> /etc/portage/package.env
