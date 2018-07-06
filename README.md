# AutoGentoo
A gentoo portage binhost manager


## Why use AutoGentoo
### Overview
AutoGentoo is a socket server that can run on any Linux kernel. It will create a chroot enviroment to your specification (pretty much a make.conf and a profile).
Instead of downloading a stage3, autogentoo will emerge the basic profile packages into a new chroot folder. This will
give you binary packages with the compile flags (CFLAGS, CXXFLAGS etc.) you want from the start. 

The advantage of a binhost
is that you can have one computer (build server) compiling packages for many different systems. This way all package building
will be kept separate from the client computer. This means if an update breaks your system (very common with Gentoo) it won't
affect your client computer, only the chroot environment on the build server.

### How it works
![alt text](https://raw.githubusercontent.com/AutoGentoo/AutoGentoo/master/resources/autogentoo.png)


### Runtime dependencies
 - pcre2
 - curl
