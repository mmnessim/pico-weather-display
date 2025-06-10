#ifndef _MY_PORTAL_H
#define _MY_PORTAL_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

void tcp_server_init();
static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static char from_hex(char c);
static void url_decode(char *dst, const char *src);

#endif
