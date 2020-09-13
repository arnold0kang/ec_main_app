/*
*
*  模块解耦代码
*
*/
#ifndef CM_MODULE_H
#define CM_MODULE_H


#include "../common/ec_define.h"

typedef EC_INT(*module_func)(EC_VOID);

typedef struct module_table_s {
	EC_CHAR *module_name;
	module_func fun;
}module_table;

#define MODULE_ADD_INIT(name,fun) \
	static const module_table entry_##fun \
	__attribute__((__section__("link_set_init_table"))) = \
        { name, fun }

#define MODULE_ADD_EXIT(name,fun) \
	static const module_table entry_##fun \
	__attribute__((__section__("link_set_exit_table"))) = \
        { name, fun }


EC_INT ec_module_init(EC_VOID);

EC_INT ec_module_exit(EC_VOID);


#define MODULE_ADD_RUN(name,fun) \
	static const module_table entry_##fun \
	__attribute__((__section__("link_set_run_table"))) = \
        { name, fun }

#define MODULE_ADD_STOP(name, fun) \
	static const module_table entry_##fun \
	__attribute__((__section__("link_set_stop_table"))) = \
        { name, fun }

EC_INT ec_module_run(EC_VOID);

EC_INT ec_module_stop(EC_VOID);

#endif
