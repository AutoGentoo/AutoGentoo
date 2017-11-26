#include <autogentoo.h>

int main (int argc, char** argv) {
    Server* server = server_new (".", "9491", 0);
    
    Host* test;
    
    StringVector* data = string_vector_new();
    string_vector_split(data, strdup("\
Kronos\n\
default/linux/amd64/13.0/desktop/gnome/systemd\n\
x86_64-pc-linux-gnu\n\
-O2 -pipe\n\
-bindist"), "\n");
    
    test = prv_host_new(server, 0, data);
    linfo ("Created host at %p", test);
    
    vector_add (server->hosts, &test);
    Connection temp;
    temp.ip = "192.168.1.160";
    temp.parent = server;
    temp.bounded_host = NULL;
    server_bind (&temp, test);
    
    server_start (server);
}