#include <autogentoo.h>

int main (int argc, char** argv) {
    Server* server = server_new (".", 9491, 0);
    
    Host* test;
    
    StringVector* data = string_vector_new();
    
    free (data->ptr);
    char* data_temp[] = {
        "Kronos",
        "default/linux/amd64/13.0/desktop/gnome/systemd",
        "x86_64-pc-linux-gnu",
        "-O2 -pipe",
        "-bindist"
    };
    
    data->ptr = data_temp;
    data->n = 5;
    
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