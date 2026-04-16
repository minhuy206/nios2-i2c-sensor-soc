#include "max30102.h"

#include "i2c_hal.h"

#define MAX30102_I2C_ADDR             0x57u

#define MAX30102_REG_INTR_STATUS_1    0x00u
#define MAX30102_REG_INTR_STATUS_2    0x01u
#define MAX30102_REG_FIFO_WR_PTR      0x04u
#define MAX30102_REG_OVF_COUNTER      0x05u
#define MAX30102_REG_FIFO_RD_PTR      0x06u
#define MAX30102_REG_FIFO_DATA        0x07u
#define MAX30102_REG_FIFO_CONFIG      0x08u
#define MAX30102_REG_MODE_CONFIG      0x09u
#define MAX30102_REG_SPO2_CONFIG      0x0Au
#define MAX30102_REG_LED1_PA          0x0Cu
#define MAX30102_REG_LED2_PA          0x0Du
#define MAX30102_REG_INTR_ENABLE_1    0x02u
#define MAX30102_REG_INTR_ENABLE_2    0x03u
#define MAX30102_REG_REV_ID           0xFEu
#define MAX30102_REG_PART_ID          0xFFu

#define MAX30102_MODE_RESET           0x40u
#define MAX30102_MODE_SPO2            0x03u

#define MAX30102_EXPECTED_PART_ID     0x15u
#define MAX30102_SAMPLE_BYTES         6u
#define MAX30102_FIFO_DEPTH           32u
#define MAX30102_RESET_RETRY_COUNT    100u
#define MAX30102_PPG_RDY_MASK         0x40u

static int max30102_write(unsigned char reg_addr, unsigned char value)
{
    int status;

    status = i2c_hal_write_reg(MAX30102_I2C_ADDR, reg_addr, value);
    if (status != I2C_HAL_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

static int max30102_read(unsigned char reg_addr, unsigned char *value)
{
    int status;

    status = i2c_hal_read_reg(MAX30102_I2C_ADDR, reg_addr, value);
    if (status != I2C_HAL_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

static int max30102_read_multi(unsigned char reg_addr, unsigned char *buf, unsigned int len)
{
    int status;

    status = i2c_hal_read_regs(MAX30102_I2C_ADDR, reg_addr, buf, len);
    if (status != I2C_HAL_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

static int max30102_read_fifo_bytes(unsigned char *buf, unsigned int len)
{
    unsigned char reg_addr;

    if ((buf == 0) || (len == 0u)) {
        return MAX30102_ERR_ARG;
    }

    reg_addr = MAX30102_REG_FIFO_DATA;
    if (i2c_hal_write(MAX30102_I2C_ADDR, &reg_addr, 1u) != I2C_HAL_OK) {
        return MAX30102_ERR_IO;
    }
    if (i2c_hal_read(MAX30102_I2C_ADDR, buf, len) != I2C_HAL_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

static int max30102_clear_interrupts(void)
{
    unsigned char dummy;

    if (max30102_read(MAX30102_REG_INTR_STATUS_1, &dummy) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_INTR_STATUS_2, &dummy) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

static int max30102_reset_fifo(void)
{
    if (max30102_write(MAX30102_REG_FIFO_WR_PTR, 0x00u) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_write(MAX30102_REG_OVF_COUNTER, 0x00u) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_write(MAX30102_REG_FIFO_RD_PTR, 0x00u) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

int max30102_read_part_id(unsigned char *part_id, unsigned char *rev_id)
{
    if ((part_id == 0) || (rev_id == 0)) {
        return MAX30102_ERR_ARG;
    }

    if (max30102_read(MAX30102_REG_PART_ID, part_id) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_REV_ID, rev_id) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }

    return MAX30102_OK;
}

int max30102_init(void)
{
    unsigned char mode_cfg;
    unsigned char part_id;
    unsigned char rev_id;
    unsigned int retry;

    if (i2c_hal_probe(MAX30102_I2C_ADDR) != I2C_HAL_OK) {
        return MAX30102_ERR_INIT;
    }

    if (max30102_write(MAX30102_REG_MODE_CONFIG, MAX30102_MODE_RESET) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }

    mode_cfg = MAX30102_MODE_RESET;
    retry = MAX30102_RESET_RETRY_COUNT;
    while ((mode_cfg & MAX30102_MODE_RESET) != 0u) {
        if (max30102_read(MAX30102_REG_MODE_CONFIG, &mode_cfg) != MAX30102_OK) {
            return MAX30102_ERR_INIT;
        }
        if (retry == 0u) {
            return MAX30102_ERR_INIT;
        }
        retry--;
    }

    if (max30102_read_part_id(&part_id, &rev_id) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (part_id != MAX30102_EXPECTED_PART_ID) {
        return MAX30102_ERR_ID;
    }

    if (max30102_write(MAX30102_REG_INTR_ENABLE_1, 0xC0u) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_INTR_ENABLE_2, 0x00u) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_FIFO_CONFIG, 0x4Fu) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_MODE_CONFIG, MAX30102_MODE_SPO2) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_SPO2_CONFIG, 0x27u) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_LED1_PA, 0x24u) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_write(MAX30102_REG_LED2_PA, 0x24u) != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }

    if (max30102_reset_fifo() != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }
    if (max30102_clear_interrupts() != MAX30102_OK) {
        return MAX30102_ERR_INIT;
    }

    (void)rev_id;
    return MAX30102_OK;
}

int max30102_read_sample(unsigned int *red, unsigned int *ir)
{
    unsigned char intr_status_1;
    unsigned char intr_status_2;
    unsigned char fifo_wr_ptr;
    unsigned char fifo_rd_ptr;
    unsigned char ovf_counter;
    unsigned char raw[MAX30102_SAMPLE_BYTES];
    unsigned int available;

    if ((red == 0) || (ir == 0)) {
        return MAX30102_ERR_ARG;
    }

    if (max30102_read(MAX30102_REG_INTR_STATUS_1, &intr_status_1) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_INTR_STATUS_2, &intr_status_2) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_FIFO_WR_PTR, &fifo_wr_ptr) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_FIFO_RD_PTR, &fifo_rd_ptr) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }
    if (max30102_read(MAX30102_REG_OVF_COUNTER, &ovf_counter) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }

    available = (unsigned int)((fifo_wr_ptr - fifo_rd_ptr) & 0x1Fu);
    if ((available == 0u) && (ovf_counter != 0u)) {
        available = 1u;
    }
    if ((available == 0u) && ((intr_status_1 & MAX30102_PPG_RDY_MASK) == 0u)) {
        return MAX30102_ERR_NOT_READY;
    }
    if (available >= MAX30102_FIFO_DEPTH) {
        available = 1u;
    }

    if (max30102_read_fifo_bytes(raw, MAX30102_SAMPLE_BYTES) != MAX30102_OK) {
        return MAX30102_ERR_IO;
    }

    *red = ((unsigned int)raw[0] << 16) | ((unsigned int)raw[1] << 8) | (unsigned int)raw[2];
    *ir = ((unsigned int)raw[3] << 16) | ((unsigned int)raw[4] << 8) | (unsigned int)raw[5];

    *red &= 0x3FFFFu;
    *ir &= 0x3FFFFu;

    return MAX30102_OK;
}
