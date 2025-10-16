
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF 8192

void usage(const char *p) {
    fprintf(stderr, "Usage: %s -h host -p port\n", p);
    exit(1);
}

int main(int argc, char **argv) {
    char *host = "127.0.0.1";
    char *port = "9000";
    int opt;
    while ((opt = getopt(argc, argv, "h:p:")) != -1) {
        switch(opt) {
            case 'h': host = optarg; break;
            case 'p': port = optarg; break;
            default: usage(argv[0]);
        }
    }

    struct addrinfo hints, *res, *rp;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port, &hints, &res) != 0) { perror("getaddrinfo"); exit(1); }
    int sock = -1;
    for (rp = res; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) continue;
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sock); sock = -1;
    }
    freeaddrinfo(res);
    if (sock < 0) { perror("connect"); exit(1); }

    // read welcome
    char buf[BUF];
    ssize_t r = recv(sock, buf, sizeof(buf)-1, 0);
    if (r > 0) { buf[r]=0; printf("%s", buf); }

    // interactive loop
    printf("Enter commands (EXIT to quit). Examples:\n");
    printf(" SELECT ALL\n SELECT WHERE <col> <value>\n INSERT <csvline>\n DELETE WHERE <col> <value>\n UPDATE WHERE <col> <value> SET <col2>=<value2>\n BEGIN TRANSACTION\n COMMIT TRANSACTION\n ROLLBACK TRANSACTION\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(buf, sizeof(buf), stdin)) break;
        // send to server
        size_t len = strlen(buf);
        if (len == 0) continue;
        if (send(sock, buf, len, 0) != (ssize_t)len) { perror("send"); break; }

        // read response (simple: read up to BUF and print; server may send more)
        ssize_t n = recv(sock, buf, sizeof(buf)-1, 0);
        if (n <= 0) {
            printf("Disconnected from server\n");
            break;
        }
        buf[n]=0;
        printf("%s\n", buf);

        // if BYE or EXIT then break
        if (strncasecmp(buf, "BYE", 3) == 0) break;
    }

    close(sock);
    return 0;
}
