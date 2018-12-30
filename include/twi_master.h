#ifndef INCLUDE_TWI_MASTER_H
#define INCLUDE_TWI_MASTER_H

#include <inttypes.h>

void twi_master_init(void);

/* Writes 'count' bytes from 'data' to address */
uint8_t twi_write_data(uint8_t address, const uint8_t *data, uint8_t count);
uint8_t twi_write_reg_data(uint8_t address, uint8_t reg, const uint8_t *data, uint8_t count);

/* Reads 'count' bytes from address into 'data' buffer */
uint8_t twi_read_data(uint8_t address, uint8_t *data, uint8_t count);
uint8_t twi_read_reg_data(uint8_t address, uint8_t reg, uint8_t *data, uint8_t count);

#endif
