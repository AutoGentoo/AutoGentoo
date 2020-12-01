//
// Created by tumbar on 12/1/20.
//

#include <sys/socket.h>
#include "tcp_client.h"
#include <netinet/in.h>
#include <sys/un.h>

int tcp_client_send_message(Address address,
                            net_type_t addr_type,
                            MessageFrame* messsage,
                            MessageFrame* reply)
{
    int sock = -1;
    if (addr_type == NETWORK_TYPE_UNIX)
    {
        sock = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, address.path, sizeof(addr.sun_path) - 1);

        if (connect(sock, (const struct sockaddr*) &addr, sizeof(addr)) != 0)
        {
            lerror("Failed to connect to unix socket '%s'", address.path);
            return -1;
        }
    }
    else
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(address.net_addr.port);
        addr.sin_addr.s_addr = htonl(address.net_addr.ip);

        if (connect(sock, (const struct sockaddr*) &addr, sizeof(addr)) != 0)
        {
            lerror("Failed to connect to tcp '%s'", address.path);
            return -1;
        }
    }

    /* Tell the server how long the message is */
    U64 total_length = sizeof(Message) + sizeof(messsage->size) + messsage->size;
    write(sock, &total_length, sizeof(total_length));

    /* Send the message */
    write(sock, messsage, sizeof(Message));

    /* Send frame information */
    write(sock, &messsage->size, sizeof(messsage->size));
    if (messsage->size)
        write(sock, messsage->data, messsage->size);

    /* Read the response length from server */
    U64 response_length = 0;
    read(sock, &response_length, sizeof(response_length));

    /* Read the message */
    read(sock, &reply, sizeof(Message));
    read(sock, &reply->size, sizeof(reply->size));

    if (reply->size)
    {
        reply->data = malloc(reply->size);
        read(sock, reply->data, reply->size);
    }
    else
        reply->data = NULL;

    return 0;
}
