#include <netinet/in.h>
#include <stddef.h>
#define MAX_SOCKET_COUNT 10

typedef struct {
    int socket_id;
    struct sockaddr *address;
    socklen_t addr_len;
    int seqnum;
} socket_entry_t;

extern socket_entry_t socket_map[MAX_SOCKET_COUNT];