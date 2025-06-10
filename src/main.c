// *********************************************************************************
// Pico Weather Display
// Hardware: Raspberry Pi Pico 2 W
// Display: Waveshare 2.9 inch e-Paper (EPD) or LCD1602 display
//
//  Pico libraries
//
#include <hardware/sync.h>
#include <pico/time.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include <stdbool.h>
#include <stdio.h>

//
// Waveshare display libraries
//
#include "DEV_Config.h"
#include "EPD_2in9b_V4.h"
#include "fonts.h"
#include "GUI_Paint.h"

//
// LCD1602 driver
//
#include "lcd1602_driver.h"

//
// Local headers
//
#include "dhcpserver.h"
#include "dnsserver.h"
#include "savecred.h"
#include "portal.h"
#include "wifi.h"
#include "message.h"

static int refresh_num = 10;

// ********************************************************************************
// Define the display type you want to use and set the other to 0
// ********************************************************************************

#define LCD 0
#define EPD 1

#if EPD

// Helpder function to update the EPD display
int display_epd(struct message *msg, UBYTE *BlackImage, UBYTE *RYImage) {

    printf("Refreshing EPD\n");

    EPD_2IN9B_V4_Init_Fast();
    //Select Image
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 5, msg->first_line, &Font20, WHITE, BLACK);
    Paint_DrawString_EN(10, 30, msg->second_line, &Font20, WHITE, BLACK);
    Paint_DrawString_EN(10, 55, msg->third_line, &Font20, WHITE, BLACK);
    Paint_DrawString_EN(10, 80, msg->fourth_line, &Font20, WHITE, BLACK);
    Paint_DrawString_EN(10, 105, msg->fifth_line, &Font20, WHITE, BLACK);

    EPD_2IN9B_V4_Display(BlackImage, RYImage);

    EPD_2IN9B_V4_Sleep();

    return 0;
}
#endif

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Wifi init failed\n");
        watchdog_reboot(0, 0, 0);
    }

    char ssid[64] = {0};
    char password[64] = {0};
    char zipcode[64] = {0};
    struct message msg = {0};

    // Try to read the SSID and password from flash memory
    // If it fails, it will enable AP mode and start the DHCP server
    // This allows the user to connect to the access point and enter credentials
    if (!read_from_flash(ssid, password, zipcode)) {
        printf("Failed to read credentials from flash\n");

        cyw43_arch_enable_ap_mode("WeatherDisplay_AP", "weather1234", CYW43_AUTH_WPA2_MIXED_PSK);

        dhcp_server_t dhcp_server;
        ip_addr_t ip, nm, gw;

        // Set the IP address for the DHCP server
        IP4_ADDR(&ip, 192, 168, 4, 1); // Example IP address for the access point
        IP4_ADDR(&nm, 255, 255, 255, 0); // Example subnet mask
        IP4_ADDR(&gw, 192, 168, 4, 1); // Example gateway
        dhcp_server_init(&dhcp_server, &ip, &nm);

        dns_server_t dns_server;
        dns_server_init(&dns_server, &ip);
        tcp_server_init();

        update_message(&msg, "Connect to ", "WeatherDisplay_AP", "to connect to wifi", "password:", "weather1234");

        #if EPD
        DEV_Module_Init();

        EPD_2IN9B_V4_Init_Fast();
        EPD_2IN9B_V4_Clear();

        //Create a new image cache named IMAGE_BW and fill it with white
        UBYTE *BlackImage, *RYImage; // Red or Yellow
        UWORD Imagesize = ((EPD_2IN9B_V4_WIDTH % 8 == 0)? (EPD_2IN9B_V4_WIDTH / 8 ): (EPD_2IN9B_V4_WIDTH / 8 + 1)) * EPD_2IN9B_V4_HEIGHT;
        if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
            printf("Failed to apply for black memory...\r\n");
            return -1;
        }
        if((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
            printf("Failed to apply for red memory...\r\n");
            return -1;
        }
        printf("NewImage:BlackImage and RYImage\r\n");
        Paint_NewImage(BlackImage, EPD_2IN9B_V4_WIDTH, EPD_2IN9B_V4_HEIGHT, 270, WHITE);
        Paint_NewImage(RYImage, EPD_2IN9B_V4_WIDTH, EPD_2IN9B_V4_HEIGHT, 270, WHITE);

        display_epd(&msg, BlackImage, RYImage);
        sleep_ms(1000);
        #endif
        while (true) {
            // Keep the program running to maintain the access point
            sleep_ms(1000);
        }
    }

    int rc = setup(ssid, password);
    if (rc == 1 || rc == 2) {
        #if LCD
        lcd_clear();
        lcd_print("Conn error");
        #endif
        // Timeout after one minute, reset credentials, and reboot
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, 4096);
        restore_interrupts(ints);
        sleep_ms(500);
        watchdog_reboot(0, 0, 0);
        return 1;
    }

    get_request(&msg);

    // EPD Initialization stuff

    #if EPD
    DEV_Module_Init();

    EPD_2IN9B_V4_Init_Fast();
    EPD_2IN9B_V4_Clear();

    //Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *BlackImage, *RYImage; // Red or Yellow
    UWORD Imagesize = ((EPD_2IN9B_V4_WIDTH % 8 == 0)? (EPD_2IN9B_V4_WIDTH / 8 ): (EPD_2IN9B_V4_WIDTH / 8 + 1)) * EPD_2IN9B_V4_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    if((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }
    printf("NewImage:BlackImage and RYImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9B_V4_WIDTH, EPD_2IN9B_V4_HEIGHT, 270, WHITE);
    Paint_NewImage(RYImage, EPD_2IN9B_V4_WIDTH, EPD_2IN9B_V4_HEIGHT, 270, WHITE);

    display_epd(&msg, BlackImage, RYImage);
    sleep_ms(1000 * 60 * 10); // Wait for 10 minutes before the first update
    #elif LCD
    lcd_init();
    lcd_clear();
    lcd_set_cursor(0, 0);

    get_request(&msg);
    lcd_print(msg.second_line);
    lcd_set_cursor(1, 0);
    lcd_print(msg.fourth_line);
    #endif

    while (true) {
        cyw43_arch_lwip_begin();
        printf("Refreshing\n");
        get_request(&msg);
        cyw43_arch_lwip_end();

        #if EPD

        DEV_Delay_ms(500); // Wait for the request to complete

        display_epd(&msg, BlackImage, RYImage);

        DEV_Delay_ms(1000 * 60 * 10); // Repeat every 10 minutes

        #elif LCD
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(msg.second_line);
        lcd_set_cursor(1, 0);
        lcd_print(msg.fourth_line);
        #endif

    }
    return 0;
}

