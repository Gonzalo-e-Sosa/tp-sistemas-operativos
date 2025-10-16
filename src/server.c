
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF 4096
#define MAX_LINE 8192

// Globals for server config
char csv_path[512] = "data.csv";
int listen_fd = -1;
sem_t active_clients; // limit concurrent
int max_clients = 5;
int backlog = 10;

// Transaction control
pthread_mutex_t tx_mutex = PTHREAD_MUTEX_INITIALIZER;
int transaction_active = 0;
int transaction_owner = -1; // socket fd of owner

// Simple logging
void logmsg(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "[server] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

// Helper: sendall
ssize_t sendall(int fd, const char *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = send(fd, buf + total, len - total, 0);
        if (n <= 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += n;
    }
    return total;
}

int starts_with(const char *s, const char *p) {
    return strncasecmp(s, p, strlen(p))==0;
}

// Trim newline
void trimnl(char *s) {
    char *p = s + strlen(s) - 1;
    while (p >= s && (*p == '\n' || *p == '\r')) { *p = 0; p--; }
}

// Read entire file into string (caller frees)
char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t r = fread(buf,1,sz,f);
    buf[r] = 0;
    fclose(f);
    return buf;
}

// Apply modifications (list of operations) by writing to temp and renaming atomically.
// For simplicity, operations are lines of text in the form we accept; but here we
// will accept full file replacement content provided by the transaction owner.
int atomic_replace_file_from_buffer(const char *path, const char *newcontent) {
    char tmp[600];
    snprintf(tmp, sizeof(tmp), "%s.tmpXXXXXX", path);
    int tmpfd = mkstemp(tmp);
    if (tmpfd < 0) return -1;
    ssize_t towrite = strlen(newcontent);
    ssize_t w = write(tmpfd, newcontent, towrite);
    if (w != towrite) { close(tmpfd); unlink(tmp); return -1; }
    fsync(tmpfd);
    close(tmpfd);
    if (rename(tmp, path) < 0) { unlink(tmp); return -1; }
    return 0;
}

// Parse CSV by simple line-splitting and search by column index or name (very simple)
void handle_select_all(int client_fd) {
    char *content = read_file(csv_path);
    if (!content) {
        sendall(client_fd, "ERROR: cannot read CSV\n", 22);
        return;
    }
    sendall(client_fd, content, strlen(content));
    free(content);
}

// SELECT WHERE <col> <value>   (col is column index starting 0 or name — we use index)
void handle_select_where(int client_fd, char *colstr, char *val) {
    char *content = read_file(csv_path);
    if (!content) {
        sendall(client_fd, "ERROR: cannot read CSV\n", 22);
        return;
    }
    char *out = malloc(strlen(content) + 1024);
    out[0]=0;
    char *line = strtok(content, "\n");
    while (line) {
        // naive CSV split by comma
        char *linecopy = strdup(line);
        int idx = 0;
        char *tok = strtok(linecopy, ",");
        int target = atoi(colstr);
        int matched = 0;
        while (tok) {
            if (idx == target) {
                if (strcmp(tok, val)==0) matched = 1;
                break;
            }
            idx++;
            tok = strtok(NULL, ",");
        }
        free(linecopy);
        if (matched) {
            strcat(out, line);
            strcat(out, "\n");
        }
        line = strtok(NULL, "\n");
    }
    if (strlen(out)==0) sendall(client_fd, "OK: no rows matched\n", 20);
    else sendall(client_fd, out, strlen(out));
    free(out);
    free(content);
}

// Simple insert: append line to CSV
int append_line_to_csv(const char *line) {
    int fd = open(csv_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) return -1;
    size_t len = strlen(line);
    if (write(fd, line, len) != (ssize_t)len) { close(fd); return -1; }
    if (write(fd, "\n", 1) != 1) { close(fd); return -1; }
    close(fd);
    return 0;
}

// Simple delete where column matches value -> create new file without matching rows
int delete_where_col_eq(int col, const char *val) {
    char *content = read_file(csv_path);
    if (!content) return -1;
    char *out = malloc(strlen(content)+1);
    out[0]=0;
    char *line = strtok(content, "\n");
    while (line) {
        char *linecopy = strdup(line);
        int idx = 0; int matched = 0;
        char *tok = strtok(linecopy, ",");
        while (tok) {
            if (idx == col) { if (strcmp(tok, val)==0) matched = 1; break; }
            idx++; tok = strtok(NULL, ",");
        }
        free(linecopy);
        if (!matched) {
            strcat(out, line);
            strcat(out, "\n");
        }
        line = strtok(NULL, "\n");
    }
    int res = atomic_replace_file_from_buffer(csv_path, out);
    free(out); free(content);
    return res;
}

// Update where column = value, set another column to newvalue
int update_where_set(int col_where, const char *val_where, int col_set, const char *val_set) {
    char *content = read_file(csv_path);
    if (!content) return -1;
    char *out = malloc(strlen(content)+1024);
    out[0]=0;
    char *line = strtok(content, "\n");
    while (line) {
        char *linecopy = strdup(line);
        // split tokens
        char *tokens[256]; int tcount=0;
        char *p = strtok(linecopy, ",");
        while (p && tcount < 256) { tokens[tcount++] = strdup(p); p = strtok(NULL, ","); }
        int matched = 0;
        if (col_where < tcount && strcmp(tokens[col_where], val_where)==0) {
            matched = 1;
            // set
            free(tokens[col_set]);
            tokens[col_set] = strdup(val_set);
        }
        // reconstruct line
        for (int i=0;i<tcount;i++) {
            strcat(out, tokens[i]);
            if (i < tcount-1) strcat(out, ",");
            free(tokens[i]);
        }
        strcat(out, "\n");
        free(linecopy);
        line = strtok(NULL, "\n");
    }
    int res = atomic_replace_file_from_buffer(csv_path, out);
    free(out); free(content);
    return res;
}

// Each client thread
void *client_thread(void *arg) {
    int client_fd = *((int*)arg);
    free(arg);

    // mark as active
    // sem already decremented by main acceptor before creating thread; we'll increment upon exit

    char buf[BUF];
    int in_transaction = 0;
    // For transaction: we will store a copy of file content at BEGIN, then allow modifications locally
    char *tx_buffer = NULL;

    logmsg("client connected fd=%d", client_fd);
    sendall(client_fd, "WELCOME to CSV server\n", 21);

    while (1) {
        ssize_t r = recv(client_fd, buf, sizeof(buf)-1, 0);
        if (r <= 0) {
            // client disconnected
            logmsg("client fd=%d disconnected (r=%zd, errno=%d)", client_fd, r, errno);
            break;
        }
        buf[r]=0;
        trimnl(buf);
        if (strlen(buf)==0) continue;

        // Commands handling
        if (starts_with(buf, "EXIT")) {
            sendall(client_fd, "BYE\n",4);
            break;
        } else if (starts_with(buf, "SELECT ALL")) {
            // check for active transaction by other
            pthread_mutex_lock(&tx_mutex);
            if (transaction_active && transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: transaction active, retry later\n", 38);
            } else {
                pthread_mutex_unlock(&tx_mutex);
                // If this client has transaction and had a tx_buffer, read from that
                if (in_transaction && tx_buffer) {
                    sendall(client_fd, tx_buffer, strlen(tx_buffer));
                } else {
                    handle_select_all(client_fd);
                }
            }
        } else if (starts_with(buf, "SELECT WHERE ")) {
            // format: SELECT WHERE <col> <value>
            char *p = buf + strlen("SELECT WHERE ");
            char colstr[32], val[256];
            if (sscanf(p, "%31s %255s", colstr, val) < 2) {
                sendall(client_fd, "ERROR: bad SELECT WHERE syntax\n",31);
                continue;
            }
            pthread_mutex_lock(&tx_mutex);
            if (transaction_active && transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: transaction active, retry later\n", 38);
            } else {
                pthread_mutex_unlock(&tx_mutex);
                // if in tx and have buffer, search in buffer
                if (in_transaction && tx_buffer) {
                    // simple search implemented by temporarily writing tx_buffer to temp file and reusing select_where logic?
                    // To keep simple: write tx_buffer to a temp file, call same logic
                    char tmp[256]; snprintf(tmp, sizeof(tmp), "%s.client%d.tmp", csv_path, client_fd);
                    FILE *f = fopen(tmp,"w");
                    if (!f) { sendall(client_fd, "ERROR: internal\n",16); continue; }
                    fprintf(f, "%s", tx_buffer);
                    fclose(f);
                    // reuse read_file trick by swapping csv_path temporarily
                    char saved[512]; strcpy(saved, csv_path);
                    strcpy(csv_path, tmp);
                    handle_select_where(client_fd, colstr, val);
                    strcpy(csv_path, saved);
                    unlink(tmp);
                } else {
                    handle_select_where(client_fd, colstr, val);
                }
            }
        } else if (starts_with(buf, "INSERT ")) {
            char *p = buf + strlen("INSERT ");
            pthread_mutex_lock(&tx_mutex);
            if (transaction_active && transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: transaction active, retry later\n", 38);
            } else {
                // if in tx: append to tx_buffer
                if (in_transaction) {
                    if (!tx_buffer) tx_buffer = strdup("");
                    size_t newlen = strlen(tx_buffer) + strlen(p) + 2;
                    tx_buffer = realloc(tx_buffer, newlen);
                    strcat(tx_buffer, p);
                    strcat(tx_buffer, "\n");
                    sendall(client_fd, "OK: inserted (in transaction)\n", 29);
                } else {
                    // append to actual file
                    if (append_line_to_csv(p) == 0) sendall(client_fd, "OK: inserted\n",12);
                    else sendall(client_fd, "ERROR: insert failed\n",21);
                }
                pthread_mutex_unlock(&tx_mutex);
            }
        } else if (starts_with(buf, "DELETE WHERE ")) {
            // DELETE WHERE <col> <value>
            char *p = buf + strlen("DELETE WHERE ");
            int col; char val[256];
            if (sscanf(p, "%d %255s", &col, val) < 2) {
                sendall(client_fd, "ERROR: bad DELETE syntax\n",24);
                continue;
            }
            pthread_mutex_lock(&tx_mutex);
            if (transaction_active && transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: transaction active, retry later\n", 38);
            } else {
                if (in_transaction) {
                    // apply to tx_buffer: naive -> rewrite tx_buffer removing matches
                    if (!tx_buffer) tx_buffer = read_file(csv_path); // start from base if null
                    if (!tx_buffer) { pthread_mutex_unlock(&tx_mutex); sendall(client_fd, "ERROR: internal\n",16); continue; }
                    char *newbuf = malloc(strlen(tx_buffer)+1); newbuf[0]=0;
                    char *line = strtok(tx_buffer, "\n");
                    while (line) {
                        char *lc = strdup(line);
                        int idx=0; char *t = strtok(lc, ","); int matched=0;
                        while (t) {
                            if (idx==col) { if (strcmp(t, val)==0) matched=1; break; }
                            idx++; t=strtok(NULL,",");
                        }
                        free(lc);
                        if (!matched) { strcat(newbuf, line); strcat(newbuf, "\n"); }
                        line = strtok(NULL, "\n");
                    }
                    free(tx_buffer);
                    tx_buffer = newbuf;
                    sendall(client_fd, "OK: delete (in transaction)\n",27);
                } else {
                    int res = delete_where_col_eq(col, val);
                    if (res==0) sendall(client_fd, "OK: deleted\n",11);
                    else sendall(client_fd, "ERROR: delete failed\n",21);
                }
                pthread_mutex_unlock(&tx_mutex);
            }
        } else if (starts_with(buf, "UPDATE WHERE ")) {
            // UPDATE WHERE <col> <value> SET <col2>=<value2>
            char *p = buf + strlen("UPDATE WHERE ");
            int colw, cols;
            char valw[256], valset[256];
            char tmpset[512];
            // parse naive
            char *setpos = strstr(p, " SET ");
            if (!setpos) { sendall(client_fd, "ERROR: bad UPDATE syntax\n",24); continue; }
            *setpos = 0;
            setpos += strlen(" SET ");
            if (sscanf(p, "%d %255s", &colw, valw) < 2) { sendall(client_fd, "ERROR: bad UPDATE WHERE part\n",29); continue; }
            // setpos format: <col2>=<value2>
            if (sscanf(setpos, "%d=%255s", &cols, valset) < 2) { sendall(client_fd, "ERROR: bad SET part\n",20); continue; }

            pthread_mutex_lock(&tx_mutex);
            if (transaction_active && transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: transaction active, retry later\n", 38);
            } else {
                if (in_transaction) {
                    if (!tx_buffer) tx_buffer = read_file(csv_path);
                    if (!tx_buffer) { pthread_mutex_unlock(&tx_mutex); sendall(client_fd, "ERROR: internal\n",16); continue; }
                    // apply update on tx_buffer (reuse update_where_set by writing tmp file)
                    char tmpname[512]; snprintf(tmpname,sizeof(tmpname), "%.480s.client%d.tmp", csv_path, client_fd);
                    FILE *f = fopen(tmpname,"w");
                    if (!f) { pthread_mutex_unlock(&tx_mutex); sendall(client_fd, "ERROR: internal\n",16); continue; }
                    fprintf(f, "%s", tx_buffer);
                    fclose(f);
                    char saved[512]; strcpy(saved, csv_path);
                    strcpy(csv_path, tmpname);
                    int res = update_where_set(colw, valw, cols, valset);
                    strcpy(csv_path, saved);
                    // read updated tmp file into tx_buffer
                    char *updated = read_file(tmpname);
                    if (updated) {
                        free(tx_buffer);
                        tx_buffer = updated;
                    }
                    unlink(tmpname);
                    if (res==0) sendall(client_fd, "OK: updated (in transaction)\n",29);
                    else sendall(client_fd, "ERROR: update failed\n",21);
                } else {
                    int res = update_where_set(colw, valw, cols, valset);
                    if (res==0) sendall(client_fd, "OK: updated\n",11);
                    else sendall(client_fd, "ERROR: update failed\n",21);
                }
                pthread_mutex_unlock(&tx_mutex);
            }
        } else if (starts_with(buf, "BEGIN TRANSACTION")) {
            pthread_mutex_lock(&tx_mutex);
            if (transaction_active) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: another transaction active\n",34);
            } else {
                transaction_active = 1;
                transaction_owner = client_fd;
                in_transaction = 1;
                // snapshot current file into tx_buffer
                if (tx_buffer) { free(tx_buffer); tx_buffer = NULL; }
                tx_buffer = read_file(csv_path);
                if (!tx_buffer) tx_buffer = strdup("");
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "OK: transaction started\n",24);
            }
        } else if (starts_with(buf, "COMMIT TRANSACTION")) {
            pthread_mutex_lock(&tx_mutex);
            if (!transaction_active || transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: no active transaction for you\n",36);
            } else {
                // apply tx_buffer to file atomically
                // Acquire file descriptor lock to avoid races with other processes (advisory)
                int fd = open(csv_path, O_RDWR | O_CREAT, 0644);
                if (fd < 0) { pthread_mutex_unlock(&tx_mutex); sendall(client_fd, "ERROR: cannot open file\n",24); continue; }
                struct flock fl;
                fl.l_type = F_WRLCK; fl.l_whence = SEEK_SET; fl.l_start = 0; fl.l_len = 0;
                if (fcntl(fd, F_SETLKW, &fl) < 0) { close(fd); pthread_mutex_unlock(&tx_mutex); sendall(client_fd, "ERROR: cannot lock file\n",24); continue; }
                // now write: replace file
                int res = atomic_replace_file_from_buffer(csv_path, tx_buffer ? tx_buffer : "");
                // unlock
                fl.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &fl);
                close(fd);
                // clear tx state
                free(tx_buffer); tx_buffer = NULL;
                transaction_active = 0;
                transaction_owner = -1;
                in_transaction = 0;
                pthread_mutex_unlock(&tx_mutex);
                if (res==0) sendall(client_fd, "OK: committed\n",13);
                else sendall(client_fd, "ERROR: commit failed\n",22);
            }
        } else if (starts_with(buf, "ROLLBACK TRANSACTION")) {
            pthread_mutex_lock(&tx_mutex);
            if (!transaction_active || transaction_owner != client_fd) {
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "ERROR: no active transaction for you\n",36);
            } else {
                // discard tx_buffer
                free(tx_buffer); tx_buffer = NULL;
                transaction_active = 0;
                transaction_owner = -1;
                in_transaction = 0;
                pthread_mutex_unlock(&tx_mutex);
                sendall(client_fd, "OK: rolled back\n",15);
            }
        } else {
            sendall(client_fd, "ERROR: unknown command\n",23);
        }
    }

    // Clean up: if this client owned a transaction, roll back and release flag
    pthread_mutex_lock(&tx_mutex);
    if (transaction_active && transaction_owner == client_fd) {
        transaction_active = 0;
        transaction_owner = -1;
        logmsg("rolled back transaction of client %d due to disconnect", client_fd);
    }
    pthread_mutex_unlock(&tx_mutex);

    if (listen_fd >= 0) {
        // release active clients slot
        sem_post(&active_clients);
    }

    close(client_fd);
    return NULL;
}

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s -h host -p port -n max_clients -m backlog -f csvfile\n", prog);
    exit(1);
}

int main(int argc, char **argv) {
    char *host = "0.0.0.0";
    char *port = "9000";
    int opt;
    while ((opt = getopt(argc, argv, "h:p:n:m:f:")) != -1) {
        switch(opt) {
            case 'h': host = optarg; break;
            case 'p': port = optarg; break;
            case 'n': max_clients = atoi(optarg); break;
            case 'm': backlog = atoi(optarg); break;
            case 'f': strncpy(csv_path, optarg, sizeof(csv_path)-1); break;
            default: usage(argv[0]);
        }
    }

    if (max_clients <= 0) max_clients = 5;
    if (backlog <= 0) backlog = 10;

    sem_init(&active_clients, 0, max_clients);

    // Setup listening socket
    struct addrinfo hints, *res, *rp;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM; hints.ai_flags = AI_PASSIVE;
    int rc = getaddrinfo(host, port, &hints, &res);
    if (rc != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc)); exit(1); }
    for (rp = res; rp; rp = rp->ai_next) {
        listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listen_fd < 0) continue;
        int yes = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(listen_fd);
        listen_fd = -1;
    }
    if (listen_fd < 0) { perror("bind"); exit(1); }
    freeaddrinfo(res);
    if (listen(listen_fd, backlog) < 0) { perror("listen"); exit(1); }
    logmsg("listening on %s:%s  max_clients=%d backlog=%d csv=%s", host, port, max_clients, backlog, csv_path);

    // accept loop
    while (1) {
        struct sockaddr_storage cliaddr; socklen_t clilen = sizeof(cliaddr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&cliaddr, &clilen);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept"); break;
        }

        // Check capacity: try sem_trywait
        if (sem_trywait(&active_clients) != 0) {
            // capacity reached -> inform client and close
            sendall(client_fd, "ERROR: server at capacity, try later\n", 36);
            close(client_fd);
            continue;
        }

        // spawn thread
        pthread_t tid;
        int *pclient = malloc(sizeof(int));
        *pclient = client_fd;
        if (pthread_create(&tid, NULL, client_thread, pclient) != 0) {
            sendall(client_fd, "ERROR: server thread create failed\n",34);
            close(client_fd);
            sem_post(&active_clients);
            free(pclient);
            continue;
        }
        pthread_detach(tid);
    }

    close(listen_fd);
    sem_destroy(&active_clients);
    return 0;
}
