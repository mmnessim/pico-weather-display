#ifndef _MY_WIFI_H
#define _MY_WIFI_H

#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <lwip/tcp.h>
#include <stdio.h>
#include <string.h>
#include "lcd1602_driver.h"
#include "message.h"

static err_t recv_callback(void *arg,
    struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t conn_callback(void *arg,
    struct tcp_pcb *tpcb, err_t err);
void get_request(struct message *msg);
int setup(const char *ssid, const char *pass);
void found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

#endif
