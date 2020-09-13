#ifndef KING_LOG_H
#define KING_LOG_H

#include <stdio.h>
#include "../common/ec_define.h"
#if 1
#include "zlog.h"
#else

#define zlog_profile()
#define dzlog_init(arg...)
#define dzlog_info(arg...)
#define dzlog_error(arg...)
#define dzlog_debug(arg...)
#define dzlog_msg(arg...)
#define dzlog_fatal(arg...)

#endif



#define LOG_OPT(MODULE, INTERFACE, ...)

EC_VOID ec_log_on (EC_VOID);
EC_VOID ec_log_off (EC_VOID);

#endif
