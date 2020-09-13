#ifndef EC_RTC_H
#define EC_RTC_H

#include "../common/ec_define.h"

EC_INT ec_rtc_init(EC_VOID);

EC_INT ec_rtc_get_time(EC_CHAR *buf, EC_INT bufsize);

EC_INT ec_rtc_get_ts(EC_CHAR *buf, EC_CHAR bufsize);

EC_INT ec_rtc_get_diff(EC_VOID);

EC_INT ec_rtc_set_time(EC_CHAR *buf);

EC_INT ec_rtc_set_diff(EC_INT ts);

EC_INT ec_rtc_deinit(EC_VOID);

EC_INT ec_rtc_save_time (EC_VOID);



#endif
