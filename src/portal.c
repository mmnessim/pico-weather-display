#include "portal.h"
#include "savecred.h"
#include "hardware/watchdog.h"

#define PORT 80


// Basic TCP Server for captive portal
// Clients are served a form to enter Wi-Fi credentials
// if form is submitted, eventually the ssid and password will be saved to flash
// and the pico will reboot to connect to the specified Wi-Fi network
void tcp_server_init() {
    struct tcp_pcb *pcb = tcp_new();
    if (pcb == NULL) {
        printf("Failed to create TCP PCB\n");
        return;
    }

    err_t err = tcp_bind(pcb, IP_ADDR_ANY, PORT);
    if (err != ERR_OK) {
        printf("Failed to bind TCP PCB: %s\n", lwip_strerr(err));
        tcp_close(pcb);
        return;
    }

    pcb = tcp_listen(pcb);
    if (pcb == NULL) {
        printf("Failed to listen on TCP PCB\n");
        return;
    }

    printf("TCP server initialized on port %d\n", PORT);

    tcp_accept(pcb, accept_callback);
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, recv_callback);
    return ERR_OK;
}

// This is where the action is in the TCP server.
// If the method is POST, it will parse the form data
// TODO : Save the SSID and password to flash memory
// Otherwise, it will serve the HTML form
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    //printf("Received data: %.*s\n", p->len, (char *)p->payload);

    char *form = "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                 "<style>"
                 "body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 0; }"
                 ".container { background: #fff; padding: 24px 16px; border-radius: 10px; max-width: 95vw; margin: 32px auto; box-shadow: 0 2px 8px rgba(0,0,0,0.08); font-size: 1.2rem; }"
                 "form { display: flex; flex-direction: column; gap: 14px; }"
                 "input[type='text'], input[type='password'] { font-size: 1.1rem; padding: 10px; border-radius: 6px; border: 1px solid #ccc; width: 100%; box-sizing: border-box; }"
                 "input[type='submit'] { font-size: 1.1rem; padding: 10px; border-radius: 6px; border: none; background: #0078d7; color: #fff; cursor: pointer; }"
                 "input[type='submit']:active { background: #005fa3; }"
                 "label { font-weight: 500; }"
                 "</style></head><body><div class='container'>"
                 "<h1>Pico Weather Display</h1>"
                 "<p>Enter your home network's wi-fi credentials below to connect the device to your network:</p>"
                 "<form method='POST' action='/'>"
                    "<label for='ssid'>SSID:</label>"
                    "<input type='text' id='ssid' name='ssid'><br>"
                    "<label for='password'>Password:</label>"
                    "<input type='password' id='password' name='password'><br>"
                    "<label for='zipcode'>Zip Code:</label>"
                    "<input type='text' id='zipcode' name='zipcode'><br>"
                    "<input type='hidden' name='action' value='submit'>"
                    "<input type='submit' value='Submit'>"
                "</form>"
                "</div></body></html>";

    char *req = (char *)p->payload;
    if (strncmp(req, "POST", 4) == 0) {
        char* ssid_start = strstr(req, "ssid=");
        char* password_start = strstr(req, "password=");
        char* zipcode_start = strstr(req, "zipcode=");

        char decoded_ssid[64] = {0};
        char decoded_password[64] = {0};
        char decoded_zipcode[64] = {0};

        if (ssid_start && password_start) {
            ssid_start += 5; // Move past "ssid="
            password_start += 9; // Move past "password="

            char *ssid_end = strchr(ssid_start, '&');
            char *password_end = strchr(password_start, '&');
            char *zipcode_end = strchr(zipcode_start, '&');

            if (ssid_start) ssid_end[0] = '\0'; // Null-terminate SSID
            if (password_start) password_end[0] = '\0'; // Null-terminate Password
            if (zipcode_start) zipcode_end[0] = '\0';

            url_decode(decoded_ssid, ssid_start);
            url_decode(decoded_password, password_start);
            url_decode(decoded_zipcode, zipcode_start);

            printf("Received SSID: %s\n", decoded_ssid);
            printf("Received Password: %s\n", decoded_password);
            printf("Received Zipcode: %s\n", decoded_zipcode);

            // TODO: Save the SSID and password to flash memory
            write_to_flash(decoded_ssid, decoded_password, decoded_zipcode);
            // After saving, we can reboot the device to connect to the new Wi-Fi network
            sleep_ms(1000); // Give some time for the write to complete
            watchdog_reboot(0, 0, 0);
        }
    }

    tcp_write(tpcb, form, strlen(form), TCP_WRITE_FLAG_COPY);
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}

// Helper function to convert a single hex character to its integer value
static char from_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0; // Invalid character
}

// Function to decode a URL-encoded string
void url_decode(char *dst, const char *src) {
    while (*src) {
        if (*src == '%') {
            if (src[1] && src[2]) {
                *dst++ = (from_hex(src[1]) << 4) | from_hex(src[2]);
                src += 3; // Skip past the %XX
            } else {
                *dst++ = '%'; // Just copy the % if not followed by two hex digits
                src++;
            }
        } else if (*src == '+') {
            *dst++ = ' '; // Convert + to space
            src++;
        } else {
            *dst++ = *src++; // Copy the character as is
        }
    }
    *dst = '\0'; // Null-terminate the decoded string
}
