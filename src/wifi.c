#include "wifi.h"
#include "message.h"
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/tcp.h>

int setup(const char *ssid, const char *pass) {
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_MIXED_PSK, 1000 * 60 *2)) {
        printf("Could not connect\n");
        return 2;
    }
    printf("Connected\n");
    //lcd_clear();
    //lcd_print("Connected");
    return 0;
}

static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    struct message *msg = (struct message *)arg;

    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Copy payload to a local buffer
    char buffer[1024] = {0};
    size_t copy_len = p->len < sizeof(buffer) - 1 ? p->len : sizeof(buffer) - 1;
    memcpy(buffer, p->payload, copy_len);
    buffer[copy_len] = '\0';

    printf("Received data: %s\n", buffer);

    // Skip HTTP headers (assumes 7 lines of headers)
    char *line = strtok(buffer, "\n");
    for (int i = 0; i < 6 && line != NULL; i++) {
        line = strtok(NULL, "\n");
    }

    if (line) {
        printf("Parsed line: %s\n", line);

        float cur = 0.0f, high = 0.0f, low = 0.0f, precip = 0.0f;
        char weather[50] = {0};

        char *token = strtok(line, " ");
        while (token != NULL) {
            char key[32], value[32];
            if (sscanf(token, "%31[^=]=%31s", key, value) == 2) {
                if (strcmp(key, "cur") == 0) {
                    cur = atof(value);
                } else if (strcmp(key, "high") == 0) {
                    high = atof(value);
                } else if (strcmp(key, "low") == 0) {
                    low = atof(value);
                } else if (strcmp(key, "precip") == 0) {
                    precip = atof(value);
                } else if (strcmp(key, "weather") == 0) {
                    strncpy(weather, value, sizeof(weather) - 1);
                }
            }
            token = strtok(NULL, " ");
        }

        printf("CURRENT: %.1f HIGH: %.1f LOW: %.1f PRECIP: %.1f WEATHER: %s\n", cur, high, low, precip, weather);

        char first_line[64], second_line[64], third_line[64], fourth_line[64], fifth_line[64];

        snprintf(first_line, sizeof(first_line), "Current: %.1fF", cur);
        snprintf(second_line, sizeof(second_line), "H: %.1fF L: %.1fF", high, low);
        snprintf(third_line, sizeof(third_line), "%s", weather);
        snprintf(fifth_line, sizeof(fifth_line), "Precip: %.1f%%", precip);

        update_message(msg, first_line, second_line, third_line, "", fifth_line);
        //lcd_clear();
        //lcd_set_cursor(0, 0);
        //lcd_print(msg->first_line);
        //lcd_set_cursor(1, 0);
        //lcd_print(msg->second_line);
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}


static err_t conn_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    const char *get_req = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    tcp_write(tpcb, get_req, strlen(get_req), TCP_WRITE_FLAG_COPY);
    tcp_arg(tpcb, arg); // Set the argument to be passed to recv_callback
    tcp_recv(tpcb, recv_callback);
    return ERR_OK;
}


void get_request(struct message *msg) {
    ip_addr_t server_ip;
    struct tcp_pcb *pcb = tcp_new();

    IP4_ADDR(&server_ip, 192, 168, 68, 68);

    tcp_arg(pcb, msg);
    tcp_connect(pcb, &server_ip, 8080, conn_callback);
}

// This doesn't work yet because TLS is not integrated
// Maybe I need mbedTLS
//void get_request(struct message *msg) {
//    const char *domain = "weather.mnessim.com";
//    ip_addr_t resolved_ip;
//
//    err_t err = dns_gethostbyname(domain, &resolved_ip, found_callback, msg);
//
//    if (err == ERR_OK) {
//        struct tcp_pcb *pcb = tcp_new();
//        tcp_arg(pcb, msg);
//        tcp_connect(pcb, &resolved_ip, 80, conn_callback);
//    } else if (err == ERR_INPROGRESS) {
//        printf("DNS resolution in progress for %s\n", domain);
//    } else {
//        printf("Could not resolve DNS\n");
//    }
//}
//
void found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr != NULL) {
        struct message *msg = (struct message *)callback_arg;
        struct tcp_pcb *pcb = tcp_new();
        tcp_arg(pcb, msg);
        tcp_connect(pcb, ipaddr, 80, conn_callback);
    }
}
