#include "lwip/pbuf.h"
#include "lwip/udp.h"

#define SOCK_BUF_SIZE 512

#define SOCK_FREE_SPACE(sock) ((sock->head - sock->tail - 1 + SOCK_BUF_SIZE) % SOCK_BUF_SIZE)

// pos: Position of this packet in buffer
// len: Length of this packet
typedef void (*sock_cb_t)(void *args, char *_sock, u16_t pos, u16_t len);

typedef struct _usock_t {
    struct udp_pcb *pcb;
    ip_addr_t d_addr, s_addr;
    u16_t d_port, s_port;

    // Circular buffer
    char buf[SOCK_BUF_SIZE];
    u16_t head, tail;
    sock_cb_t cb;
    void* cb_args;
} usock_t;

// Initialize a socket
// Use s_ip = IP_ANY_TYPE to bind all ip addr.
// cb is a callback function which will be called when a packet arrives,
// with cb_args
usock_t* sock_init(char *d_ip, u16_t d_port, char *s_ip, u16_t s_port, sock_cb_t cb, void *cb_args);

// De-initialize a socket, and release its memory
void sock_close(usock_t *sock);

// Send LEN bytes of data into socket.
// Returns 0 if success.
int sock_write(usock_t *sock, char *data, int len);

// Read at most LEN bytes from socket into buf, no blocking.
// Return number of bytes read.
int sock_read(usock_t *sock, char *buf, int len);

// The callback function when a packet arrives
void recv_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

// For debug
void sock_status(usock_t *sock);
