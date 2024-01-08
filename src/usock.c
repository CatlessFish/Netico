#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "usock.h"

void recv_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    // arg should be sock
    usock_t *sock = arg;
    if (addr->addr != (sock->d_addr).addr || port != sock->d_port) {
        // Not from our destination
        // return;
    }
    if (p->tot_len > SOCK_FREE_SPACE(sock)) {
        // This packet is too big
        return;
    }
    // Only support single pbuf for now
    memcpy(&sock->buf[sock->tail], p->payload, p->len);
    u16_t pos = sock->tail;
    sock->tail = (sock->tail + p->len) % SOCK_BUF_SIZE;

    if (sock->cb) sock->cb(sock->cb_args, (char *)sock, pos, p->len);

    // DEBUG
    printf("Received a packet from %s:%d, length = %d\n", ipaddr_ntoa(addr), port, p->len);
}

void sock_status(usock_t *sock) {
    if (sock == 0) printf("Socket Status: Invalid socket\n");
    printf("Socket Status:\n");
    printf("--Des %s:%d\n", ipaddr_ntoa(&sock->d_addr), sock->d_port);
    printf("--Src %s:%d\n", ipaddr_ntoa(&sock->s_addr), sock->s_port);
    printf("--Buf: head=%d, tail=%d\n", sock->head, sock->tail);
}

usock_t* sock_init(char *d_ip, u16_t d_port, char *s_ip, u16_t s_port, sock_cb_t cb, void *cb_args) {
    usock_t *sock = malloc(sizeof(usock_t));
    if (sock == 0) return 0;
    memset(sock, 0, sizeof(usock_t));
    if (d_ip == 0 || ipaddr_aton(d_ip, &sock->d_addr) != 1) {
        printf("Sock init: Invalid Des IP\n");
        return 0;
    }
    if (s_ip == 0 || ipaddr_aton(s_ip, &sock->s_addr) != 1) {
        printf("Sock init: Invalid Src IP\n");
        return 0;
    }
    sock->d_port = d_port;
    sock->s_port = s_port;
    sock->cb = cb;

    sock->pcb = udp_new();
    if (sock->pcb == 0) {
        printf("Sock init: create pcb failed\n");
        return 0;
    }

    err_t err = udp_bind(sock->pcb, &sock->s_addr, sock->s_port);
    if (err != ERR_OK) {
        if (err == ERR_USE) printf("Sock init: Address in use\n");
        else printf("Sock init: bind failed\n");
        udp_remove(sock->pcb);
        return 0;
    }

    // Set up a callback for receive
    udp_recv(sock->pcb, recv_cb, sock);

    // sock_status(sock);

    return sock;
}

void sock_close(usock_t *sock) {
    if (sock == 0) return;
    if (sock->pcb != 0) udp_remove(sock->pcb);
    free(sock);
}

int sock_write(usock_t *sock, char *data, int len) {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    memset((char *)p->payload, 0, len);
    strncpy((char *)p->payload, data, len);
    err_t er = udp_sendto(sock->pcb, p, &sock->d_addr, sock->d_port);
    pbuf_free(p);
    if (er != ERR_OK) {
        printf("[sock] Send failed.\n");
        sock_status(sock);
        return -1;
    }
    printf("[sock] Send %d bytes\n", len);
    return 0;
}

int sock_read(usock_t *sock, char *buf, int len) {
    int cnt = 0;
    len = (len == 0) ? SOCK_BUF_SIZE : len; 
    while (sock->head != sock->tail && len > 0) {
        buf[cnt++] = sock->buf[sock->head];
        sock->head = (sock->head + 1) % SOCK_BUF_SIZE;
        len--;
    }
    return cnt;
}