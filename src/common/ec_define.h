#ifndef EC_DEFINE_H
#define EC_DEFINE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include "zlog.h"

//#define FACTORY_MODE 							1


#define EC_SUCCESS								0
#define EC_FAILURE								-1
#define ECBUFSIZE								8192
#define EC_NULL								NULL


#define EC_FALSE								0
#define EC_TRUE									1

typedef int										EC_INT;
typedef char									EC_CHAR;
typedef int										EC_BOOL;
typedef unsigned int							EC_UINT;
typedef double									EC_DOUBLE;
typedef float									EC_FLOAT;
typedef long									EC_LONG;
typedef long long								EC_LLONG;
typedef unsigned  long long						EC_ULLONG;
typedef unsigned char							EC_UCHAR;
typedef short									EC_SHORT;
typedef unsigned short							EC_USHORT;
typedef unsigned long							EC_ULONG;
typedef long									EC_LONG;



#define EC_VOID                               void

#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))

#define EC_MEMSET(ARG) do {\
	memset(ARG, 0, sizeof(*ARG));}while(0)

#define EC_MEMSET_2(ARG, LEN) do {\
	memset(ARG, 0, LEN);}while(0)


#define CHECK_NULL(PTR, LABEL) do {\
	if (PTR == EC_NULL) goto failed_##LABEL; }while(0)


#define CHECK_FAILED(RET, LABEL) do {\
	if (RET == EC_FAILURE) goto failed_##LABEL;}while(0)

#define CHECK_SUCCESS(RET, VALUE, LABEL) do {\
	if (RET != VALUE) goto failed_##LABEL;}while(0)

#define EC_MALLOC(PTR) do {\
	PTR = calloc(1, sizeof (*PTR)); \
	assert(PTR);\
	}while(0)

#define EC_MALLOC_WITH_SIZE(PTR, SIZE) do {\
	PTR=calloc (1, SIZE); \
	assert(PTR);  \
	}while(0)


#define EC_FREE(PTR) do {\
	if (PTR) {free(PTR); PTR=EC_NULL;}} while(0)

#define EC_ERR_PTR   EC_CHAR *errStr = NULL

#define EC_ERR_SET(ERRMSG) do { \
	if (!errStr) { errStr = ERRMSG;} \
	} while (0)

#define EC_ERR_OUT() do { \
	if(errStr) {dzlog_error(errStr);}}while(0)
#define CLOSE_FD(FD) do { \
	if (FD > 0) {close(FD);} FD = -1;}while(0)

#define EC_DUMP_NUM(VALUE)			fprintf(stderr,  #VALUE "-->%d\n", VALUE)
#define EC_DUMP_STR(VALUE)			fprintf(stderr,  #VALUE "-->%s\n", VALUE)


#define EC_FILE_NAME_MAX_LEN				(1024)

#define EC_START_THREAD(RET, PID, FUNC) do {\
	RET= pthread_create(PID, NULL,  FUNC, NULL);} while(0)

#define HISI_AAC							1
//#define USE_FFMPEG							1

#endif