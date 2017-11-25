#include <stdio.h>
#include <stdlib.h>
#include <server.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <handle.h>
#include <errno.h>

static Connection** finished_connection;

Server* server_new (char* location, char* port, server_t opts) {
    Server* out = malloc (sizeof (Server));
    out->connections = vector_new (sizeof (Connection*), REMOVE | UNORDERED);
    out->hosts = vector_new (sizeof (Host*), REMOVE | UNORDERED);
    out->host_bindings = small_map_new(15, 5);
    out->location = strdup (location);
    out->opts = opts;
    strcpy(out->port, port);
    
    return out;
}

Connection* connection_new (Server* server, int conn_fd) {
    if (conn_fd < 0) {
        lwarning ("accept() error");
        return NULL;
    }
    
    Connection* out = mmap(NULL, sizeof (Connection), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    out->parent = server;
    out->fd = conn_fd;
    
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof (struct sockaddr_in);
    int res = getpeername (out->fd, (struct sockaddr*)&addr, &addr_size);
    out->ip = inet_ntoa(addr.sin_addr);
    
    Host** temp = small_map_get (out->parent->host_bindings, out->ip);
    
    if (temp != NULL) { // Cant dereference NULL
        out->bounded_host = *temp;
    }
    else {
        out->bounded_host = NULL;
    }
    
    return out;
}

void server_start (Server* server) {
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    
    finished_connection = mmap(NULL, sizeof (Connection*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    int listenfd = server_init(server->port);

    linfo("Server started on port %s", server->port);

    if (server->opts & DAEMON) {
        daemonize (server->location);
    }

    signal(SIGCHLD, kill_finished);

    addrlen = sizeof(clientaddr);
    
    while (1) { // Main accept loop
        Connection* current_conn = connection_new (server, accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen));
        
        if (current_conn == NULL)
            continue;
        
        pid_t res_pid;
        if ((res_pid = fork ()) == -1)
            exit (-1);
        else if (res_pid == 0) {
            current_conn->pid = getpid();
            server_respond(current_conn);
            *finished_connection = current_conn;
            _exit (0);
        }
    }
}

int server_init (char* port) {
    struct addrinfo hints, *res, *p;
    int listenfd;
    
    // getaddrinfo for host
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        lerror("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, 0);
        if (listenfd == -1)
            continue;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
            lerror("setsockopt(SO_REUSEADDR) failed");
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p == NULL) {
        lerror("socket() or bind()");
        switch (errno) {
            case EADDRINUSE:
            lerror ("Port %s in use", port);
            break;
            case EACCES:
            lerror ("Permission denied");
            break;
            case ENETUNREACH:
            lerror ("Network unreachable");
            break;
        }
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    
    if (listen(listenfd, 64) != 0) {
        lerror("listen() error");
        exit(1);
    }
    
    return listenfd;
}

void connection_free (Connection* conn) {
    if (conn->status != CLOSED) {
        shutdown(conn->fd, SHUT_RDWR);
        close(conn->fd);
        conn->status = CLOSED;
    }
    
    free (conn->request);
    free (conn->ip);
    // Don't unmap here (done in kill_finished)
}

void kill_finished (int sig) {
    if ((*finished_connection)->pid > 0) {
        waitpid ((*finished_connection)->pid, 0, WNOHANG);
        (*finished_connection)->pid = -1;
    }

    if ((*finished_connection)->fd > 2) { // Dont close stdout, stdin or stderr
        close ((*finished_connection)->fd);
    }
    
    munmap (*finished_connection, sizeof(Connection*));
}

void server_respond (Connection* conn) {
    
    /* Read the request */
    conn->request = malloc (2048);
    ssize_t total_read, current_bytes = 0;
    size_t buffer_size = 2048;
    total_read += current_bytes = read (conn->fd, conn->request, 2048);
    while (current_bytes == 2048) {
        buffer_size += 2048;
        conn->request = realloc (conn->request, buffer_size);
        current_bytes = read (conn->fd, conn->request + total_read, 2048);
        total_read += current_bytes;
    }
    
    if (total_read < 0) { // receive error
        lerror("recv() error");
        conn->status = SERVER_ERROR;
        return;
    }
    else if (total_read == 0) { // receive socket closed
        lwarning("Client disconnected upexpectedly.");
        conn->status = FAILED;
        return;
    }
    else {
        conn->status = CONNECTED;
    }
    
    char* args[4]; // Written to by parse_request
    
    char* request_line;
    int split_i = strchr (conn->request, '\n') - conn->request;
    request_line = malloc (split_i);
    strncpy (request_line, conn->request, split_i);
    
    SHFP call = parse_request (request_line, args);
    
    linfo("handle %s on pid %d (%s)", conn->ip, conn->pid, request_line);
    
    response_t res = (*call) (conn, args, split_i + 1);
    linfo ("request %d: %s (%d)", conn->pid, res.message, res.code);
}

void server_bind (Connection* conn, Host** host) {
    small_map_insert (conn->parent->host_bindings, conn->ip, &host);
}

void daemonize(char* _cwd) {
    pid_t pid, sid;
    int fd;

    /* already a daemon */
    if (getppid () == 1)
        return;

    /* Fork off the parent process */
    pid = fork ();
    if (pid < 0) {
        exit(1);
    }

    if (pid > 0) {
        printf ("Forked to pid: %d\n", (int)pid);
        printf ("Moving to background\n");
        fflush (stdout);
        exit(0); /*Killing the Parent Process*/
    }

    /* At this point we are executing as the child process */

    /* Create a new SID for the child process */
    sid = setsid ();
    if (sid < 0) {
        exit(1);
    }

    /* Change the current working directory. */
    if ((chdir (_cwd)) < 0) {
        exit(1);
    }


    fd = open ("/dev/null",O_RDWR, 0);

    if (fd != -1) {
        dup2 (fd, STDIN_FILENO);
        dup2 (fd, STDOUT_FILENO);
        dup2 (fd, STDERR_FILENO);

        if (fd > 2) {
            close (fd);
        }
    }

    /*resettign File Creation Mask */
    umask (027);
}