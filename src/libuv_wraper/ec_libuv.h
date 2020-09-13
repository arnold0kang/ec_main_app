#ifndef KINGH_LIBUV_H
#define KINGH_LIBUV_H

#include "../common/ec_define.h"
#include "uv.h"



uv_loop_t  *ec_libuv_default(EC_VOID);

#define ec_looper \
	 ec_libuv_default()


EC_INT ec_libuv_init(EC_VOID);

EC_VOID ec_libuv_run(EC_VOID);

EC_VOID ec_libuv_exit(EC_VOID);



#endif // !KINGH_LIBUV_H

