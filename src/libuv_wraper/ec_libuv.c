#include "ec_libuv.h"

uv_loop_t  *ec_libuv_default(EC_VOID)
{
	return uv_default_loop();
}


EC_INT ec_libuv_init(EC_VOID)
{
	
	return EC_SUCCESS;
}

EC_VOID ec_libuv_run(EC_VOID)
{
	uv_run(ec_looper, UV_RUN_DEFAULT);
}

EC_VOID ec_libuv_exit(EC_VOID)
{
	uv_loop_close(ec_looper);
	return;
}