#include <autogentoo.h>
#include <download.h>

int main (int argc, char** argv) {
    download ("http://distfiles.gentoo.org/releases/arm/autobuilds/current-stage3-armv7a/stage3-armv7a_hardfp-20161129.tar.bz2", "temp.html", SHOW_PROGRESS);
    
    /*
    Server* server = read_server ("/media/autogentoo");
    server_start (server);*/
}