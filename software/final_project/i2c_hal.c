#include "i2c_hal.h"

#include "altera_avalon_i2c.h"

/* Open once and keep a handle for all transactions. */
static ALT_AVALON_I2C_DEV_t *g_i2c_dev = 0;

static int i2c_hal_is_valid_addr(unsigned char dev_addr)
{
    return dev_addr <= 0x7Fu;
}

static int i2c_hal_map_status(ALT_AVALON_I2C_STATUS_CODE status)
{
    if (status == ALT_AVALON_I2C_SUCCESS) {
        return I2C_HAL_OK;
    }

    if (status == ALT_AVALON_I2C_BUSY) {
        return I2C_HAL_ERR_BUSY;
    }
    if (status == ALT_AVALON_I2C_NACK_ERR) {
        return I2C_HAL_ERR_NACK;
    }
    if (status == ALT_AVALON_I2C_TIMEOUT) {
        return I2C_HAL_ERR_TIMEOUT;
    }
    if (status == ALT_AVALON_I2C_BAD_ARG) {
        return I2C_HAL_ERR_ARG;
    }

    return I2C_HAL_ERR_IO;
}

int i2c_hal_init(void)
{
    if (g_i2c_dev != 0) {
        return I2C_HAL_OK;
    }

    g_i2c_dev = alt_avalon_i2c_open("/dev/i2c_0");
    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }

    return I2C_HAL_OK;
}

int i2c_hal_set_speed(unsigned int hz)
{
    ALT_AVALON_I2C_MASTER_CONFIG_t cfg;
    ALT_AVALON_I2C_STATUS_CODE status;

    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }

    if ((hz != 100000u) && (hz != 400000u)) {
        return I2C_HAL_ERR_ARG;
    }

    alt_avalon_i2c_master_config_get(g_i2c_dev, &cfg);
    status = alt_avalon_i2c_master_config_speed_set(g_i2c_dev, &cfg, hz);
    if (status != ALT_AVALON_I2C_SUCCESS) {
        return i2c_hal_map_status(status);
    }

    alt_avalon_i2c_master_config_set(g_i2c_dev, &cfg);
    return I2C_HAL_OK;
}

int i2c_hal_probe(unsigned char dev_addr)
{
    unsigned char dummy = 0;
    ALT_AVALON_I2C_STATUS_CODE status;

    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }
    if (!i2c_hal_is_valid_addr(dev_addr)) {
        return I2C_HAL_ERR_ARG;
    }

    alt_avalon_i2c_master_target_set(g_i2c_dev, dev_addr);
    status = alt_avalon_i2c_master_rx(g_i2c_dev, &dummy, 1, ALT_AVALON_I2C_NO_INTERRUPTS);

    return i2c_hal_map_status(status);
}

int i2c_hal_write_reg(unsigned char dev_addr, unsigned char reg_addr, unsigned char value)
{
    unsigned char txbuf[2];
    ALT_AVALON_I2C_STATUS_CODE status;

    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }
    if (!i2c_hal_is_valid_addr(dev_addr)) {
        return I2C_HAL_ERR_ARG;
    }

    txbuf[0] = reg_addr;
    txbuf[1] = value;

    alt_avalon_i2c_master_target_set(g_i2c_dev, dev_addr);
    status = alt_avalon_i2c_master_tx(g_i2c_dev, txbuf, 2, ALT_AVALON_I2C_NO_INTERRUPTS);

    return i2c_hal_map_status(status);
}

int i2c_hal_read_reg(unsigned char dev_addr, unsigned char reg_addr, unsigned char *value)
{
    ALT_AVALON_I2C_STATUS_CODE status;

    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }
    if ((value == 0) || !i2c_hal_is_valid_addr(dev_addr)) {
        return I2C_HAL_ERR_ARG;
    }

    alt_avalon_i2c_master_target_set(g_i2c_dev, dev_addr);
    status = alt_avalon_i2c_master_tx_rx(
        g_i2c_dev,
        &reg_addr,
        1,
        value,
        1,
        ALT_AVALON_I2C_NO_INTERRUPTS
    );

    return i2c_hal_map_status(status);
}

int i2c_hal_read_regs(unsigned char dev_addr, unsigned char start_reg, unsigned char *buf, unsigned int len)
{
    ALT_AVALON_I2C_STATUS_CODE status;

    if (g_i2c_dev == 0) {
        return I2C_HAL_ERR_INIT;
    }
    if ((buf == 0) || (len == 0u) || !i2c_hal_is_valid_addr(dev_addr)) {
        return I2C_HAL_ERR_ARG;
    }

    alt_avalon_i2c_master_target_set(g_i2c_dev, dev_addr);
    status = alt_avalon_i2c_master_tx_rx(
        g_i2c_dev,
        &start_reg,
        1,
        buf,
        len,
        ALT_AVALON_I2C_NO_INTERRUPTS
    );

    return i2c_hal_map_status(status);
}

const char *i2c_hal_strerror(int status)
{
    if (status == I2C_HAL_OK) {
        return "OK";
    }
    if (status == I2C_HAL_ERR_INIT) {
        return "controller init failed";
    }
    if (status == I2C_HAL_ERR_ARG) {
        return "invalid argument";
    }
    if (status == I2C_HAL_ERR_BUSY) {
        return "bus busy";
    }
    if (status == I2C_HAL_ERR_NACK) {
        return "no acknowledge from device";
    }
    if (status == I2C_HAL_ERR_TIMEOUT) {
        return "transfer timeout";
    }
    if (status == I2C_HAL_ERR_IO) {
        return "generic I2C I/O error";
    }

    return "unknown I2C error";
}
