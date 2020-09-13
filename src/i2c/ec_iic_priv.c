#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <hi_i2c.h>
#include "../log/ec_log.h"

#include "ec_iic_priv.h"

#define EC_IIC_ADDR_WIDTH			0x01
#define EC_IIC_DEV_ADDR			(0xA0 >> 1)

EC_INT ec_iic_read(EC_INT fd, EC_INT addr, EC_CHAR *buf, EC_INT readLen)
{
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msg[2];
	char tmpBuf[4] = { '\0' };
	int ret;

	

#if 1
	ret = ioctl(fd, I2C_SLAVE_FORCE, EC_IIC_DEV_ADDR);
	if (ret < 0) {
		dzlog_error("CMD_SET_I2C_SLAVE error!");
		goto failed_0;
	}


	msg[0].addr = EC_IIC_DEV_ADDR;
	msg[0].flags = 0;
	msg[0].len = EC_IIC_ADDR_WIDTH;
	msg[0].buf = tmpBuf;

	msg[1].addr = EC_IIC_DEV_ADDR;
	msg[1].flags = 0;
	msg[1].flags |= I2C_M_RD;
	msg[1].len = 1;
	msg[1].buf = tmpBuf;

	rdwr.msgs = &msg[0];
	rdwr.nmsgs = (__u32)2;
#endif

	int curr_addr;
	int i;
	for (curr_addr = addr, i = 0; i < readLen; i++, curr_addr++)
	{
		tmpBuf[0] = (curr_addr) & 0xff;
		ret = ioctl(fd, I2C_RDWR, &rdwr);
		if (ret != 2)
		{
			dzlog_error("read i2c failed, %d ", ret);
			perror("read ii2: ");
			goto failed_0;
		}
	//	dzlog_debug("read %x %x", curr_addr, tmpBuf[0]);
		buf[i] = tmpBuf[0];
	}


	return EC_SUCCESS;
failed_0:
	return EC_FAILURE;
}

EC_INT ec_iic_write(EC_INT fd, EC_INT addr, EC_CHAR *buf, EC_CHAR writeLen)
{
	char tmp[2];
	int ret;
#if 1
	ret = ioctl(fd, I2C_SLAVE_FORCE, EC_IIC_DEV_ADDR);
	if (ret < 0)
	{
		dzlog_error("set i2c device address failed");
		perror("iic error: ");
		goto failed_0;
	}
#endif
	int i;
	for (i = 0; i < writeLen; i++)
	{
		tmp[0] = (addr+i) & 0xff;
		tmp[1] = buf[i];

		ret = write(fd, tmp, 2);
		if (ret < 0)
		{
			dzlog_error("write i2c failed");
			goto failed_0;
		}
		usleep(30000);
	}
	
	return EC_SUCCESS;
failed_0:
	return EC_FAILURE;
}
