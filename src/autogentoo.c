#include <autogentoo.h>

int main (int argc, char** argv) {
    Server* server = read_server ("/media/autogentoo");
    server_start (server);
}