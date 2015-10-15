# AutoGentoo
AutoGentoo is a graphical installer for gentoo
This installer is aimed at users with low experience with Gentoo and users who need to quickly install a gentoo system

#Installation & Dependencies
AutoGentoo requires many packages these must installed or it may not work properly

(Gentoo Packages)
* e2fsprogs (FAT filesystem support)
* parted
* gparted (optional)
* gtk+
* python2.7
* pygtk
* pygobject
* gobject
* webkit-gtk
* ntfs-3g (NTFS support)
* gksu
* sudo

Install all these programs with
 ```
 emerge -a e2fsprogs parted gparted gtk+ python2.7 pygtk pygobject gobject webkit-gtk ntfs-3g sudo gksu
 ```