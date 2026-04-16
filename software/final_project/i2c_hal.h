#ifndef I2C_HAL_H
#define I2C_HAL_H

/* HAL return codes */
#define I2C_HAL_OK           (0)
#define I2C_HAL_ERR_INIT     (-1)
#define I2C_HAL_ERR_ARG      (-2)
#define I2C_HAL_ERR_BUSY     (-3)
#define I2C_HAL_ERR_NACK     (-4)
#define I2C_HAL_ERR_TIMEOUT  (-5)
#define I2C_HAL_ERR_IO       (-6)

int i2c_hal_init(void);
int i2c_hal_set_speed(unsigned int hz);
int i2c_hal_probe(unsigned char dev_addr);
int i2c_hal_write(unsigned char dev_addr, const unsigned char *buf, unsigned int len);
int i2c_hal_read(unsigned char dev_addr, unsigned char *buf, unsigned int len);
int i2c_hal_write_reg(unsigned char dev_addr, unsigned char reg_addr, unsigned char value);
int i2c_hal_read_reg(unsigned char dev_addr, unsigned char reg_addr, unsigned char *value);
int i2c_hal_read_regs(unsigned char dev_addr, unsigned char start_reg, unsigned char *buf, unsigned int len);
const char *i2c_hal_strerror(int status);

#endif
