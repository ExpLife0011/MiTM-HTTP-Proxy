
#include "tcp.h"

struct addrinfo* getTCPInfo(char *hostname, char* port){
    struct addrinfo hints, *res;
    int ec;
    memset(&hints, 0, sizeof hints);
    // Use IPv4
    hints.ai_family = AF_INET;
    // Use TCP
    hints.ai_socktype = SOCK_STREAM;
    // Lookup host
    if ( (ec = getaddrinfo(hostname, port, &hints, &res)) != 0)
        die(gai_strerror(ec));
    return res;
}

int Listen(void *addr, char *port){
    struct addrinfo *res, *p;
    int sockfd;

    res = getTCPInfo(addr, port);
    int yes = 1;
    // loop through results until a suitable bind is made.
    for (p = res; p!=NULL; p = res->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("bad file descriptor");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(yes), sizeof(int))==-1)
            die("setsockopt");

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("bind");
            close(sockfd);
            continue;
        }
        break;
    }
    if (p == NULL)
        die("server failed to bind");
    if (listen(sockfd, 12) == -1) {
        die("Could not listen");
    }
    freeaddrinfo(res); // all done with this structure
    return sockfd;
}

int Connect(char *hostname, int _port){
    int sockfd;
    if (_port>65535 || _port <1){
        die("invalid port number: %d", _port);
    }
    char port[6];
    sprintf(port, "%d", _port);
    struct addrinfo *p, *res;
    res = getTCPInfo(hostname, port);
    // Loop through until a proper socket is found
    for (p = res; p != (struct addrinfo*)0; p=p->ai_next){
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))<0){
            close(sockfd);
            perror("connect\n");
            continue;
        }
        break;
    }
    if (p == (struct addrinfo*) 0)
        die("Connect: could not find a sockfd");
    freeaddrinfo(res);
    // establish connection
    if ( connect(sockfd, p->ai_addr, p->ai_addrlen) < 0 )
        die("connect");
    return sockfd;
}


int hostIsAlive(char* host){
    struct addrinfo hints, *info = (struct addrinfo*)0;
    int gai_result;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    int ret = 0;
    if ((gai_result = getaddrinfo(host, "http", &hints, &info)) != 0) {
        ret = gai_result;
    }
    if (info != (struct addrinfo*) 0){
        freeaddrinfo(info);
    }
    return ret;
}


unsigned int getIpInt(char* ip){
    struct in_addr addr;
    inet_aton(ip, &addr);
    return addr.s_addr;
}

