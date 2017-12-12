#include <download.h>
#include <sys/ioctl.h>
#include <stdio.h>

response_t download (char* url, down_progress p) {
    response_t out;
    
    char* protocol_splt = strchr (url, "://");
    char protocal[16];
    strncpy (protocal, url, protocol_splt - url);
    
    if (strncmp (protocal, "http") != 0) {
        return INTERNAL_ERROR;
    }
    
    url += 3;
    
    int sockfd;
    struct sockaddr_in dest;
    char buffer[MAXBUF];
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket");
        exit(errno);
    }
    
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT_FTP);
    if ( inet_aton(SERVER_ADDR, &dest.sin_addr.s_addr) == 0 ) {
        perror(SERVER_ADDR);
        exit(errno);
    }
    
    if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 ) {
        perror("Connect ");
        exit(errno);
    }
    
    bzero(buffer, MAXBUF);
    
    size_t trans = write (sockfd, )
    
    close(sockfd);
}