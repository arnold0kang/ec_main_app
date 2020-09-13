#include <pthread.h>
#include "ec_event.h"
#include "../libuv_wraper/ec_libuv.h"
#include "../log/ec_log.h"


/***************************static define*******************************/
typedef struct event_node_s
{
	ec_event_handle   *handle;
	EC_BOOL    needDel;
	struct event_node_s *next;
}eventNode;

typedef struct
{
    eventCb beforeEvent;
    eventCb afterEvent;
    eventNode *next;
}eventNodeHeader;

typedef struct
{
	ec_event_type type;
}event_arg;

static EC_VOID callEventHandles(ec_event_type eventType);
static EC_VOID event_add_new(eventNode *newNode);
static eventNode *find_node(ec_event_handle *handle);
static EC_VOID del_node(eventNode *node);
static EC_VOID free_node (eventNode *node);
static EC_VOID onEvent(uv_work_t  *req);
static EC_VOID afterEvent(uv_work_t *req, EC_INT status);
/***************************static define*******************************/

static eventNodeHeader eventNodeheaders[EC_EVENT_ALL_CNT];
static uv_rwlock_t rwLock;



EC_INT ec_event_init(EC_VOID)
{
	EC_INT i = 0;
	for (i = 0; i < EC_EVENT_ALL_CNT; i++)
	{
		eventNodeheaders[i].next = EC_NULL;
        eventNodeheaders[i].beforeEvent= EC_NULL;
        eventNodeheaders[i].afterEvent= EC_NULL;
	}
	if (uv_rwlock_init(&rwLock) != 0)
	{
		dzlog_error("init rw lock failed");
		return EC_FAILURE;
	}
	return EC_SUCCESS;
}

EC_INT ec_event_deinit(EC_VOID)
{
	EC_INT i = 0;
	eventNode *current;
	eventNode *next;
	for (i = 0; i < EC_EVENT_ALL_CNT; i++)
	{
		current = next = NULL;
		for (current = eventNodeheaders[i].next; current != EC_NULL; current = next)
		{
			next = current->next;
			EC_FREE(current->handle);  //free handle
			EC_FREE(current); //free node
		}
		
	}
	uv_rwlock_destroy(&rwLock);
	return EC_SUCCESS;
}

EC_VOID ec_event_broadcast(ec_event_type eventType)
{
	event_arg *arg;
	EC_MALLOC(arg);
	arg->type = eventType;
	uv_work_t *req;
	EC_MALLOC(req);

	req->data = (void*)arg;
	uv_queue_work(ec_looper, req, onEvent, afterEvent);

	return;
failed_1:

failed_0:
	return;
}

ec_event_handle *ec_event_new(ec_event_type type)
{
	ec_event_handle *handle;
	EC_ERR_PTR;
	EC_MALLOC(handle);
	handle->eventType = type;
	handle->priority = 100; //default priority

	return handle;

failed_0:
	EC_ERR_SET("malloc failed");
	EC_ERR_OUT();

	return EC_NULL;
}

EC_VOID *ec_event_attache(ec_event_handle *handle)
{
	EC_ERR_PTR;
	eventNode *newNode = NULL;

	CHECK_NULL(handle, 0);
	EC_MALLOC(newNode);

	newNode->handle = handle;
	newNode->needDel = EC_FALSE;

	event_add_new(newNode);



	return (EC_VOID *)newNode;

failed_1:
	EC_ERR_SET("malloc failed");
failed_0:
	EC_ERR_SET("handle is null");
	EC_ERR_OUT();

	return EC_NULL;
}

EC_VOID ec_event_del(EC_VOID *handle)
{
	((eventNode *)handle)->needDel = EC_TRUE;

	return;
}
#if 0
EC_VOID *cm_host_event_reg_listener(eventCb listener, cmh_event_arg *arg, EC_INT priority)
{
	eventNode *newNode = NULL;
	EC_MALLOC(newNode, sizeof(eventNode), 1);
	//set
	newNode->arg = arg;
	newNode->listener = listener;
	newNode->priority = priority;
	
	LOCK();
	event_add_new(newNode);
	UNLOCK();

	return newNode;

failed_1:
	return EC_NULL;
}
#endif


EC_INT ec_event_set_before (ec_event_type type, eventCb callback)
{
    if (eventNodeheaders[type].beforeEvent)
    {
        return EC_FAILURE;
    }
    eventNodeheaders[type].beforeEvent = callback;

    return EC_SUCCESS;
}
EC_INT ec_event_set_after (ec_event_type type, eventCb callback)
{
    if(eventNodeheaders[type].afterEvent)
    {
        return EC_FAILURE;
    }
    eventNodeheaders[type].afterEvent = callback;

    return EC_SUCCESS;
}


static EC_VOID callEventHandles(ec_event_type eventType)
{
	eventNode *node = NULL;

    if (eventNodeheaders[eventType].beforeEvent)
    {
        eventNodeheaders[eventType].beforeEvent(EC_NULL);
    }
	uv_rwlock_rdlock(&rwLock);
	//call event befor first
	for (node = eventNodeheaders[eventType].next; node != EC_NULL; node = node->next)
	{
		if (node->handle->eventCallback)
        {
            node->handle->eventCallback(node->handle);
            if (node->needDel)
            {
                del_node(node);
            }
        }
        else
        {
            dzlog_error("find event handle but callback is null");
        }

	}
    uv_rwlock_rdunlock(&rwLock);
	if (eventNodeheaders[eventType].afterEvent)
    {
        eventNodeheaders[eventType].afterEvent(EC_NULL);
    }



	return;
}

static EC_VOID event_add_new(eventNode *newNode)
{
	uv_rwlock_wrlock(&rwLock);

	if (eventNodeheaders[newNode->handle->eventType].next == EC_NULL)
	{
		eventNodeheaders[newNode->handle->eventType].next = newNode;
	}
	else
	{
		if (newNode->handle->priority > eventNodeheaders[newNode->handle->eventType].next->handle->priority)
		{
			newNode->next = eventNodeheaders[newNode->handle->eventType].next;
			eventNodeheaders[newNode->handle->eventType].next = newNode->next;
		}
		else
		{
			eventNode *node = EC_NULL;
			for (node = eventNodeheaders[newNode->handle->eventType].next; node->next != EC_NULL; node = node->next)
			{
				if (newNode->handle->priority > node->next->handle->priority)
				{
					newNode->next = node->next;
					node->next = newNode;
					break;
				}
			}
			if (node->next == NULL)
			{
				node->next = newNode;
			}
		}
		
	}

	uv_rwlock_wrunlock(&rwLock); 

	return;
}


static eventNode *find_node(ec_event_handle *arg)
{
	eventNode *current = EC_NULL;
	uv_rwlock_rdlock(&rwLock);
	for (current = eventNodeheaders[arg->eventType].next; current != EC_NULL; current = current->next)
	{
		if (current->handle == arg)
		{
			break;
		}
	}
	uv_rwlock_rdunlock(&rwLock);
	return current;
}
static EC_VOID del_node(eventNode *delNode)
{
	eventNode *current;
	eventNode *prive;
	eventNode *targetNode = (eventNode *)delNode;

    dzlog_debug("deleting node ");
	for (current = eventNodeheaders[targetNode->handle->eventType].next;
		current != NULL; current = current->next)
	{
		if (current == targetNode)
		{
			break;
		}
		prive = current;
	}
	if (current)
	{
		if (current == eventNodeheaders[targetNode->handle->eventType].next)
		{
			eventNodeheaders[targetNode->handle->eventType].next = current->next;
		}
		else
		{
			prive->next = current->next;
		}
		free_node(current);
	}



	return;
}

static EC_VOID free_node (eventNode *node)
{
    dzlog_debug("free node");
	if (node)
	{
		if (node->handle)
		{
			EC_FREE(node->handle);
		}
		EC_FREE(node);
	}
}

static EC_VOID onEvent(uv_work_t  *req)
{
	event_arg *arg = (event_arg *)(req->data);
	callEventHandles(arg->type);

	return;
}
static EC_VOID afterEvent(uv_work_t *req, EC_INT status)
{
	event_arg *arg = (event_arg *)(req->data);
	EC_FREE(arg);
	EC_FREE(req);

	return;
}