#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../common/ec_define.h"
#include "../log/ec_log.h"
#ifdef EC_TIME_USE_RTC
#include "ec_rtc.h"
#include "hi_rtc.h"


#define EC_RTC_DEV_PATH			"/dev/hi_rtc"
#endif
#include "hi_rtc.h"

static struct
{
	EC_INT fd;

}rtc_stat;

const char *weekStr[] =
{
	"Sun.",
	"Mon.",
	"Tue.",
	"Wed.",
	"Thu.",
	"Fri.",
	"Sat."
};


EC_INT ec_rtc_init(EC_VOID)
{
#ifdef EC_TIME_USE_RTC

	EC_ERR_PTR;
	rtc_stat.fd = open(EC_RTC_DEV_PATH, O_RDWR);
	CHECK_FAILED(rtc_stat.fd, -1, 1);

	return EC_SUCCESS;

failed_1:
	EC_ERR_SET("open rtc failed");
	EC_ERR_OUT();
	return EC_FAILURE;
#else
	return EC_SUCCESS;
#endif
}

#ifdef EC_TIME_USE_RTC

static EC_INT rtc_get_time(rtc_time_t *tm)
{

	EC_MEMSET(tm);

	CHECK_FAILED(ioctl(rtc_stat.fd, HI_RTC_RD_TIME, &tm), -1, 1);

	return EC_SUCCESS;
failed_1:
	return EC_FAILURE;
}
#endif


EC_INT ec_rtc_get_time(EC_CHAR *buf, EC_INT bufsize)
{
#ifdef EC_TIME_USE_RTC

	rtc_time_t tm;
	EC_ERR_PTR;
	
	
	CHECK_FAILED(rtc_get_time(&tm), EC_FAILURE, 1);
	snprintf(buf, bufsize - 1, "%d-%02d-%02d %02d:%02d:%02d %s", tm.year,
		tm.month, tm.date, tm.hour, tm.minute, tm.second, weekStr[tm.weekday]);

	return EC_SUCCESS;

failed_1:
	EC_ERR_SET("get rtc time failed");
	EC_ERR_OUT();
	return EC_FAILURE;
#else
	time_t ts = time(NULL);
	CHECK_FAILED(ts, 1);
	struct tm *tmpTm = gmtime(&ts);
	CHECK_NULL(tmpTm, 1);
	
	strftime(buf, 512, "%Y-%m-%d %H:%M:%S", tmpTm);

	return EC_SUCCESS;
failed_1:
	return EC_FAILURE;

#endif
}

EC_INT ec_rtc_get_ts(EC_CHAR *buf, EC_CHAR bufsize)
{
#ifdef EC_TIME_USE_RTC
	rtc_time_t tm;
	EC_ERR_PTR;

	CHECK_FAILED(rtc_get_time(&tm), EC_FAILURE, 1);


	snprintf(buf, bufsize - 1, "%d%02d%02d%02d%02d%02d", tm.year,
		tm.month, tm.date, tm.hour, tm.minute, tm.second);

	return EC_SUCCESS;

failed_1:
	EC_ERR_SET("get rtc time failed");
	EC_ERR_OUT();
	return EC_FAILURE;
#else
	time_t ts = time(NULL);
	CHECK_FAILED(ts, 1);
	struct tm *tmpTm = gmtime(&ts);
	CHECK_NULL(tmpTm, 1);

	strftime(buf, 512, "%Y%m%d%H%M%S", tmpTm);

	return EC_SUCCESS;
failed_1:
	return EC_FAILURE;
#endif
}

EC_INT ec_rtc_get_diff(EC_VOID)
{
#ifdef EC_TIME_USE_RTC

	rtc_time_t tm;
	EC_ERR_PTR;

	CHECK_FAILED(rtc_get_time(&tm), EC_FAILURE, 1);




failed_1:
	return EC_FAILURE;
#else

	return (EC_INT)time(NULL);
#endif
}

EC_INT ec_rtc_set_time(EC_CHAR *buf)
{
	return EC_FAILURE;
}

EC_INT ec_rtc_set_diff(EC_INT ts)
{
	struct timeval tv = { ts, 0 };
	settimeofday(&tv, NULL);
   // ec_rtc_save_time();

	return EC_SUCCESS;
}

EC_INT ec_rtc_deinit(EC_VOID)
{
	return EC_SUCCESS;
}


#define DEV_NAME			"/dev/hi_rtc"

EC_INT ec_rtc_save_time (EC_VOID)
{

    rtc_time_t rtc_tm;
    time_t ts = time(NULL);
    struct tm  *tm = gmtime(&ts);

    memset(&rtc_tm, 0, sizeof(rtc_time_t));

    rtc_tm.date = tm->tm_mday;
    rtc_tm.hour = tm->tm_hour;
    rtc_tm.minute = tm->tm_min;
    rtc_tm.month = tm->tm_mon + 1;
    rtc_tm.second = tm->tm_sec;
    rtc_tm.weekday = tm->tm_wday+1;
    rtc_tm.year = tm->tm_year + 1900;
#if 0
    printf("to hard\n");
	printf("year %d\n", rtc_tm.year);
	printf("month %d\n", rtc_tm.month);
	printf("date %d\n", rtc_tm.date);
	printf("hour %d\n", rtc_tm.hour);
	printf("minute %d\n", rtc_tm.minute);
	printf("second %d\n", rtc_tm.second);
	printf("weekday %d\n", rtc_tm.weekday);
#endif
    int fd = open(DEV_NAME, O_RDWR);
    if (fd < 0) {
        return -1;
    }

    ioctl(fd, HI_RTC_SET_TIME, &rtc_tm);

    close(fd);

    return 0;
}
