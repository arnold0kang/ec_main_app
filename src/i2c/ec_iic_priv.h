#ifndef EC_IIC_PRIV_H
#define EC_IIC_PRIV_H
#include "../common/ec_define.h"



EC_INT ec_iic_read(EC_INT fd, EC_INT addr, EC_CHAR *buf, EC_INT readLen);

EC_INT ec_iic_write(EC_INT fd, EC_INT addr, EC_CHAR *buf, EC_CHAR writeLen);


#endif
