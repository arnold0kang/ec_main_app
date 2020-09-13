#include <stdio.h>

#include "ec_module.h"



EC_INT ec_module_init(EC_VOID)
{
	extern   module_table __start_link_set_init_table;
	extern   module_table __stop_link_set_init_table;
	module_table *tmpTable;

	for (tmpTable = &__start_link_set_init_table; tmpTable != &__stop_link_set_init_table; tmpTable++) {
		if ((*tmpTable->fun)() == EC_FAILURE)
		{
			printf("init module %s failed", tmpTable->module_name);
			return EC_FAILURE;
		}
	}

	return EC_SUCCESS;
}

EC_INT ec_module_exit(EC_VOID)
{
	extern   module_table __start_link_set_exit_table;
	extern   module_table __stop_link_set_exit_table;
	module_table *tmpTable;

	for (tmpTable = &__start_link_set_exit_table; tmpTable != &__stop_link_set_exit_table; tmpTable++) {
		if ((*tmpTable->fun)() == EC_FAILURE)
		{
			printf("exit module %s failed", tmpTable->module_name);
			return EC_FAILURE;
		}
	}

	return EC_SUCCESS;
}

EC_INT ec_module_run(EC_VOID)
{
	extern   module_table __start_link_set_run_table;
	extern   module_table __stop_link_set_run_table;
	module_table *tmpTable;

	for (tmpTable = &__start_link_set_run_table; tmpTable != &__stop_link_set_run_table; tmpTable++) {
		if ((*tmpTable->fun)() == EC_FAILURE)
		{
			printf("run module %s failed", tmpTable->module_name);
			return EC_FAILURE;
		}
	}

	return EC_SUCCESS;
}

EC_INT ec_module_stop(EC_VOID)
{
	extern   module_table __start_link_set_stop_table;
	extern   module_table __stop_link_set_stop_table;
	module_table *tmpTable;

	for (tmpTable = &__start_link_set_stop_table; tmpTable != &__stop_link_set_stop_table; tmpTable++) {
		if ((*tmpTable->fun)() == EC_FAILURE)
		{
			printf("stop module %s failed", tmpTable->module_name);
			return EC_FAILURE;
		}
	}

	return EC_SUCCESS;
}

