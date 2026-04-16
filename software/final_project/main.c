#include <stdio.h>
#include <unistd.h>

#include "i2c_hal.h"
#include "max30102.h"

#define MAX30102_I2C_ADDR     0x57u
#define PROBE_RETRY_COUNT     3u
#define PROBE_RETRY_DELAY_US  20000u
#define SAMPLE_POLL_DELAY_US  10000u

static void print_probe_help(int status)
{
    printf("probe 0x%02x failed: %d (%s)\n",
        MAX30102_I2C_ADDR,
        status,
        i2c_hal_strerror(status));

    if ((status == I2C_HAL_ERR_NACK) || (status == I2C_HAL_ERR_IO) || (status == I2C_HAL_ERR_TIMEOUT)) {
        printf("check sensor power, GND, SDA, SCL, and pull-up resistors\n");
        printf("check that the sensor address is 0x57 and the bus speed is 100 kHz\n");
    }
}

static void scan_i2c_bus(void)
{
    unsigned char addr;
    int status;
    int found = 0;

    printf("scanning I2C bus for responding devices...\n");

    for (addr = 0x08u; addr <= 0x77u; addr++) {
        status = i2c_hal_probe(addr);
        if (status == I2C_HAL_OK) {
            printf("device responded at 0x%02x\n", addr);
            found = 1;
        }
    }

    if (!found) {
        printf("no I2C device responded on the bus\n");
    }
}

static int probe_max30102(void)
{
    unsigned int attempt;
    int status;

    for (attempt = 0u; attempt < PROBE_RETRY_COUNT; attempt++) {
        status = i2c_hal_probe(MAX30102_I2C_ADDR);
        if (status == I2C_HAL_OK) {
            return I2C_HAL_OK;
        }

        printf("probe attempt %lu/%lu failed\n",
            (unsigned long)(attempt + 1u),
            (unsigned long)PROBE_RETRY_COUNT);
        usleep(PROBE_RETRY_DELAY_US);
    }

    print_probe_help(status);
    scan_i2c_bus();
    return status;
}

int main(void)
{
    int status;
    unsigned char part_id = 0;
    unsigned char rev_id = 0;
    unsigned int red = 0;
    unsigned int ir = 0;

    printf("MAX30102 raw FIFO reader (Nios II)\n");

    status = i2c_hal_init();
    if (status != I2C_HAL_OK) {
        printf("i2c_hal_init failed: %d\n", status);
        return 1;
    }

    status = i2c_hal_set_speed(100000);
    if (status != I2C_HAL_OK) {
        printf("i2c_hal_set_speed(100000) failed: %d\n", status);
        return 1;
    }

    status = probe_max30102();
    if (status != I2C_HAL_OK) {
        return 1;
    }

    printf("probe 0x%02x: OK\n", MAX30102_I2C_ADDR);

    status = max30102_read_part_id(&part_id, &rev_id);
    if (status != MAX30102_OK) {
        printf("max30102_read_part_id failed: %d\n", status);
        return 1;
    }

    printf("part_id=0x%02x rev_id=0x%02x\n", part_id, rev_id);

    status = max30102_init();
    if (status != MAX30102_OK) {
        printf("max30102_init failed: %d\n", status);
        return 1;
    }

    printf("sensor configured, polling FIFO...\n");

    while (1) {
        status = max30102_read_sample(&red, &ir);
        if (status == MAX30102_OK) {
            printf("red=%lu ir=%lu\n", (unsigned long)red, (unsigned long)ir);
        } else if (status != MAX30102_ERR_NOT_READY) {
            printf("max30102_read_sample failed: %d\n", status);
            return 1;
        }

        usleep(SAMPLE_POLL_DELAY_US);
    }
}
