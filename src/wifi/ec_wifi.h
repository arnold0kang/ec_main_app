#ifndef EC_WIFI_H
#define EC_WIFI_H

#include "../common/ec_define.h"
#include "../dev_stat/ec_dev_state_define.h"


EC_INT ec_wifi_on (EC_VOID);
EC_INT ec_wifi_off (EC_VOID);

EC_VOID ec_wifi_restart (EC_VOID);

EC_BOOL ec_wifi_is_on (EC_VOID);


#endif
