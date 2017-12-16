#include <autogentoo.h>
#include <download.h>

int main (int argc, char** argv) {
    download ("https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html", "temp.html", SHOW_PROGRESS);
    
    /*
    Server* server = read_server ("/media/autogentoo");
    server_start (server);*/
}