#ifndef LCD1602_DRIVER_H
#define LCD1602_DRIVER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C Address of the PCF8574 backpack
#define LCD_ADDR 0x27

// I2C Pins
#define SDA_PIN 4
#define SCL_PIN 5

// LCD Commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// Flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// Enable bit
#define En 0x04
#define Rw 0x02
#define Rs 0x01

// Function declarations
void lcd_init();
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_clear();
void lcd_print(const char *str);
void lcd_write(uint8_t value);
void lcd_toggle_enable(uint8_t val);
void lcd_new_line();
void lcd_set_cursor(uint8_t row, uint8_t col);

#endif // LCD1602_DRIVER_H
