#include <autogentoo.h>

int main (int argc, char** argv) {
    Server* server = read_server (".");
    server_start (server);
    server_free (server);
}
