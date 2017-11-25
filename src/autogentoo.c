#include <autogentoo.h>

int main (int argc, char** argv) {
    Server* server = server_new ("/media/autogentoo", "9491", 0);
    server_start (server);
}