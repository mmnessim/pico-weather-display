#include "wifi.h"
#include "message.h"
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

        char string[64], second_line[64], precip_line[64];
        snprintf(precip_line, sizeof(precip_line), "Precip: %.1f%%", precip);
        snprintf(string, sizeof(string), "C:%.1fF H:%.1fF", cur, high);
        snprintf(second_line, sizeof(second_line), "%s", weather);

        update_message(msg, "Today's Weather", string, "Condition:", second_line, precip_line);
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
    const char *get_req = "POST / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
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
