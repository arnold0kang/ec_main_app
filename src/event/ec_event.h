/*
 * desp: 内部事件广播管理器
 *
 *
 */
#ifndef CM_HOST_EVENT_H
#define CM_HOST_EVENT_H

#include "../common/ec_define.h"
#include "ec_event_define.h"



typedef   EC_VOID (*eventCb)(EC_VOID *arg);

typedef struct
{
    ec_event_type eventType;
	EC_USHORT priority;
	eventCb  eventCallback;
	EC_VOID *data;
}ec_event_handle;

EC_INT ec_event_init(EC_VOID);

EC_INT ec_event_deinit(EC_VOID);

EC_VOID ec_event_broadcast(ec_event_type eventType);

#define BROAD_CAST(et) \
	ec_event_broadcast(et)

//EC_VOID *cm_host_event_reg_listener(eventCb listener, cm_host_event_arg *arg, EC_INT priority);

ec_event_handle *ec_event_new(ec_event_type type);

EC_VOID *ec_event_attache(ec_event_handle *handle);

EC_VOID ec_event_del(EC_VOID *handle);

EC_INT ec_event_set_before (ec_event_type type, eventCb callback);
EC_INT ec_event_set_after (ec_event_type type, eventCb callback);

#endif