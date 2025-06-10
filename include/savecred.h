#ifndef _SAVE_CRED_H
#define _SAVE_CRED_H

#include <stdio.h>
#include <string.h>
#include "pico/stdio.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/flash.h"

#define FLASH_TARGET_OFFSET PICO_FLASH_SIZE_BYTES - 4096
#define FLASH_HEADER "FLASH_OK:"
#define FLASH_HEADER_LEN 9

void write_to_flash(const char *ssid, const char *password);
bool read_from_flash(char *ssid, char *password);

#endif
