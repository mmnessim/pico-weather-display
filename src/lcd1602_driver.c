#include "lcd1602_driver.h"

void lcd_init() {
    // Initialize I2C at 100kHz
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Wait 50 ms to power up
    sleep_ms(50);

    // Set to 4 bit mode
    lcd_send_command(0x03);
    lcd_send_command(0x03);
    lcd_send_command(0x03);
    lcd_send_command(0x02);

    // Other setup
    lcd_send_command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
    lcd_send_command(LCD_CLEARDISPLAY);
    lcd_send_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
    sleep_ms(200);

}

void lcd_send_command(uint8_t cmd) {
    // Split the 8bit Byte into two 4bit chunks
    uint8_t highnib = cmd & 0xf0;
    uint8_t lownib = (cmd << 4) & 0xf0;

    // send first part
    lcd_write(highnib | LCD_BACKLIGHT);
    lcd_toggle_enable(highnib | LCD_BACKLIGHT);

    // send second part
    lcd_write(lownib | LCD_BACKLIGHT);
    lcd_toggle_enable(lownib | LCD_BACKLIGHT);
}

void lcd_write(uint8_t value) {
    i2c_write_blocking(i2c0, LCD_ADDR, &value, 1, false);
}

void lcd_toggle_enable(uint8_t value) {
    sleep_us(500);
    lcd_write(value | En);
    sleep_us(500);
    lcd_write(value & ~En);
    sleep_us(500);
}


void lcd_send_data(uint8_t data) {
    uint8_t highnib = data & 0xf0;
    uint8_t lownib = (data << 4) & 0xf0;
    lcd_write(highnib | Rs | LCD_BACKLIGHT);
    lcd_toggle_enable(highnib | Rs | LCD_BACKLIGHT);
    lcd_write(lownib | Rs | LCD_BACKLIGHT);
    lcd_toggle_enable(lownib | Rs | LCD_BACKLIGHT);
}


void lcd_print(const char *str) {
    int i = 0;
    while (*str) {
        lcd_send_data(*str++);
        i ++;
        if (str[i] == '\n') {
            lcd_new_line();
        }
    }
}

void lcd_clear() {
    lcd_send_command(LCD_CLEARDISPLAY);
    sleep_ms(2);
}

void lcd_new_line() {
    lcd_send_command(0xC0);
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    if (row == 0) {
        lcd_send_command(LCD_SETDDRAMADDR | (col & 0x0F));
    } else if (row == 1) {
        lcd_send_command(LCD_SETDDRAMADDR | (0x40 + (col & 0x0F)));
    }
}
