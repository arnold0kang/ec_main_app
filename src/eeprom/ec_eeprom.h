#ifndef EC_IIC_H
#define EC_IIC_H


#include "../common/ec_define.h"



EC_INT king_iic_start(EC_VOID);

EC_INT king_iic_stop(EC_VOID);

EC_INT king_iic_get_conf (EC_VOID);

EC_VOID ec_update_time_from_eeprom(void);


#endif
