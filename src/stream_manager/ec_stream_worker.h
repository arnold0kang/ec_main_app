#ifndef EC_STREAM_H
#define EC_STREAM_H

#include "../common/ec_define.h"
#include "../dev_stat/ec_dev_state_define.h"


EC_BOOL ec_stream_worker_is_run (EC_VOID);

EC_VOID ec_stream_worker_run (EC_VOID);

EC_VOID ec_stream_worker_stop (EC_VOID);

#endif
