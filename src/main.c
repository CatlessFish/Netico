#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "usock.h"
#include "econsole.h"

#define DEBUG_printf(msg) printf(msg)

int getline(char *buf, int maxlen) {
    char c;
    int i = 0;
    while((c = getchar()) != 13) { // CR is 13
        putchar(c);
        // printf("%d", c);
        if (c != 127) buf[i++] = c;
        else {
            // 127 is Backspace
            if (i > 0) i--;
        }
        if (i >= maxlen) break;
    }
    if (i < maxlen) buf[i] = '\0';
    printf("\r\n");
    return i;
}

int ep_init() {
    if(DEV_Module_Init()!=0) return -1;

	EPD_2in13_V3_Init();
    EPD_2in13_V3_Clear();

    return console_init(EPD_2in13_V3_HEIGHT, EPD_2in13_V3_WIDTH, 0, 0, &Font12);
}

void ep_down() {
    EPD_2in13_V3_Init();
    EPD_2in13_V3_Clear();
	
    EPD_2in13_V3_Sleep();
    console_destruct();
    DEV_Delay_ms(2000);//important, at least 2s
    DEV_Module_Exit();
}

int wifi_up() {
    if (cyw43_arch_init()) {
        DEBUG_printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\nID:%s\nPW:%s\n", WIFI_SSID, WIFI_PASSWORD);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }
    return 0;
}

void wifi_down() {
    cyw43_arch_deinit();
}

void print_wifi_status() {
    uint8_t mac[6];
    int status = 0;
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
    status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    printf("Mac Address:");
    for (int i = 0; i < 6; i++) {
        printf("%02x");
        if (i < 5) printf(":");
    }
    printf("\r\n");

    switch (status)
    {
    case CYW43_LINK_NOIP:
        printf("Link error: Linked but no IP\n");
        break;
    case CYW43_LINK_UP :
        printf("Linked with IP address\n");
        break;
    
    default:
        printf("Link status: %d\n", status);
    }
}

void sock_cb(void* args, char *_sock, u16_t pos, u16_t len) {
    args = 0; // Avoid unuse
    char tmp[128];
    if (len > 127) len = 127;
    usock_t *sock = (usock_t *) _sock;
    memcpy(tmp, &sock->buf[pos], len);
    tmp[len] = '\0';
    console_push(tmp);
}

int main() {

    stdio_init_all();

    char buf[64];
    int WIFI_STATUS = 0;
    usock_t *sock = 0;
    while (1) {
        getline(buf, 64);
        if (strcmp(buf, "upWIFI") == 0) {
            if (WIFI_STATUS) {
                ;
            } else {
                if (wifi_up() == 0) WIFI_STATUS = 1;
            }
            if (sock == 0) {
                sock = sock_init("192.168.1.21", 5555, "0.0.0.0", 4444, sock_cb, 0);
            }
            ep_init();
        }
        if (strcmp(buf, "downWIFI") == 0) {
            if (sock) sock_close(sock);
            wifi_down();
            ep_down();
        }
        if (strcmp(buf, "statusWIFI") == 0) {
            print_wifi_status();
            sock_status(sock);
        }

        if (WIFI_STATUS && sock) {
            sock_write(sock, buf, strlen(buf));
        }
    }
}
