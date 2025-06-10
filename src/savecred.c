#include "savecred.h"

const uint8_t *flash_target = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

void write_to_flash(const char *ssid, const char *password, const char *zipcode) {
    uint8_t buffer[FLASH_PAGE_SIZE];
    snprintf((char *)buffer, sizeof(buffer), "%sssid=%s&password=%s&zipcode=%s", FLASH_HEADER, ssid, password, zipcode);

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, 4096);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, 63);
    restore_interrupts(ints);
}

bool read_from_flash(char *ssid, char *password, char* zipcode) {
    if (memcmp(flash_target, FLASH_HEADER, FLASH_HEADER_LEN) != 0) {
        return false; // Flash header not found
    }

    const char *data = (const char *)(flash_target + FLASH_HEADER_LEN);
    // Read up to newline for SSID, then up to newline for password, allowing spaces
    sscanf(data, "ssid=%63[^&]&password=%63[^&]&zipcode=%63s", ssid, password, zipcode);
    return true;
}
