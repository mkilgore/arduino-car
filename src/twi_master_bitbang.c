
#include "common.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "twi_master.h"

/*
 * This is a bitbanged version of I2C (IE. TWI). We're doing bitbanging instead
 * of using the hardware I2C cause for some reason on the car, the ultrasonic
 * sensor is on the same pins as the hardware I2C.
 */

#define SDA_DDR    DDRC
#define SDA_DDR_N  DDC0
#define SDA_PORT   PORTC
#define SDA_PORT_N PORTC0
#define SDA_PIN    PINC
#define SDA_PIN_N  PINC0

#define SCL_DDR    DDRC
#define SCL_DDR_N  DDC1
#define SCL_PORT   PORTC
#define SCL_PORT_N PORTC1
#define SCL_PIN    PINC
#define SCL_PIN_N  PINC1

static struct twi_state {
    uint8_t send_restart :1;
} twi_state;

static inline void twi_delay(void)
{
    /* This delay gives us around a 100Khz speed. Since we have a separate clock (SCL),
     * it doesn't need to be exact */
    _delay_us(4.7);
}

static inline uint8_t scl_read(void)
{
    return !!(SCL_PIN & _BV(SCL_PIN_N));
}

static inline void scl_high(void)
{
    cbi(SCL_DDR, SCL_DDR_N);
}

static inline void scl_low(void)
{
    sbi(SCL_DDR, SCL_DDR_N);
}

static inline uint8_t sda_read(void)
{
    return !!(SDA_PIN & _BV(SDA_PIN_N));
}

static inline void sda_high(void)
{
    cbi(SDA_DDR, SDA_DDR_N);
}

static inline void sda_low(void)
{
    sbi(SDA_DDR, SDA_DDR_N);
}

void twi_master_init(void)
{
    /* Setup for open-drain configuration */
    cbi(SDA_PORT, SDA_PORT_N);
    cbi(SCL_PORT, SCL_PORT_N);

    scl_high();
    sda_high();
}

static void twi_send_start_cond(void)
{
    if (twi_state.send_restart) {
        /* Generate a restart condition instead of a regular start */
        sda_high();
        twi_delay();
        scl_high();

        /* The slave may stretch the clock to request more time by holding it low */
        while (!scl_read())
            _delay_us(10);

        twi_delay();
    }

    /* Start condition - SDA goes low while SCL is high */
    sda_low();
    twi_delay();
    scl_low();

    /* This triggers a restart to be sent if this function is called without this being reset */
    twi_state.send_restart = 1;
}

static void twi_send_stop_cond(void)
{
    sda_low();
    twi_delay();

    scl_high();

    while (!scl_read())
        _delay_us(10);

    sda_high();
    twi_delay();

    /* We just sent a stop condition, so we can't send a restart anymore */
    twi_state.send_restart = 0;
}

static void twi_write_bit(int bit)
{
    if (bit)
        sda_high();
    else
        sda_low();

    twi_delay();

    scl_high();
    twi_delay();

    while (!scl_read())
        _delay_us(10);

    scl_low();
}

static uint8_t twi_read_bit(void)
{
    uint8_t bit;

    /* This is High-Z, so the slave can drive SDA */
    sda_high();

    twi_delay();

    scl_high();

    while (!scl_read())
        _delay_us(10);

    twi_delay();

    bit = sda_read();

    scl_low();

    return bit;
}

static uint8_t twi_write_byte(uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        twi_write_bit(byte & 0x80);
        byte <<= 1;
    }

    /* This is the ACK */
    return twi_read_bit();
}

static uint8_t twi_read_byte(int nack)
{
   uint8_t result = 0;
   uint8_t i;

   for (i = 0; i < 8; i++)
       result |= twi_read_bit() << (7 - i);

   /* ACK */
   twi_write_bit(nack);

   return result;
}

uint8_t twi_write_data(uint8_t address, const uint8_t *data, uint8_t count)
{
    uint8_t i;

    twi_send_start_cond();
    twi_write_byte(address << 1);

    for (i = 0; i < count; i++)
        twi_write_byte(data[i]);

    twi_send_stop_cond();

    return 0;
}

uint8_t twi_read_data(uint8_t address, uint8_t *data, uint8_t count)
{
    uint8_t i;

    twi_send_start_cond();
    twi_write_byte((address << 1) | 0x01);

    for (i = 0; i < count - 1; i++)
        data[i] = twi_read_byte(0);

    data[i] = twi_read_byte(1);

    twi_send_stop_cond();

    return 0;
}

uint8_t twi_write_reg_data(uint8_t address, uint8_t reg, const uint8_t *data, uint8_t count)
{
    uint8_t i;

    twi_send_start_cond();
    twi_write_byte(address << 1);
    twi_write_byte(reg);

    for (i = 0; i < count; i++)
        twi_write_byte(data[i]);

    twi_send_stop_cond();

    return 0;
}

uint8_t twi_read_reg_data(uint8_t address, uint8_t reg, uint8_t *data, uint8_t count)
{
    uint8_t i;

    twi_send_start_cond();
    twi_write_byte(address << 1);
    twi_write_byte(reg);

    twi_send_start_cond();
    twi_write_byte((address << 1) | 0x01);

    for (i = 0; i < count - 1; i++)
        data[i] = twi_read_byte(0);

    data[i] = twi_read_byte(1);

    twi_send_stop_cond();

    return 0;

}

