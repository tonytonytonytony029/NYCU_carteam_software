#ifndef _I2C_EEPROM_H
#define _I2C_EEPROM_H

#include "ch.h"
#include "hal.h"

void eepromInit(void);
int eepromRead(uint16_t addr, size_t s, void *buf);
int eepromWrite(uint16_t addr, size_t s, void *buf);
void eepTest(void);

void at24eep_init(I2CDriver *i2cp,uint8_t page_size, uint32_t page_count, uint8_t address, uint8_t length_byte);

#endif