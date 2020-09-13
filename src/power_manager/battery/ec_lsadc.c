#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asm/socket.h>

#include "../../common/ec_define.h"
#include "ec_lsadc.h"
#include "../../log/ec_log.h"


#ifdef EC_BAT_USE_LSADC
#define LSADC_PATH              "/dev/lsadc"
#define LSADC_IOCTL_READ _IOR('K', 0x01, unsigned int)
#define EC_LSADC_MAX				1000.0
#define EC_LSADC_MIN				857.0
#define EC_LSADC_BASE				(EC_LSADC_MAX-EC_LSADC_MIN)
#define EC_LSADC_GETCNT				5


EC_FLOAT king_lsadc_read(EC_VOID)
{
#if 1
	EC_FLOAT ret = 0.0;
	EC_INT minIndex = 0;
	EC_INT maxIndex = 0;
	EC_INT value[EC_LSADC_GETCNT] = { 0 };
	EC_INT i;
    EC_INT lsadcFd;


    lsadcFd = open(LSADC_PATH, 0644);
    CHECK_FAILED(lsadcFd, 0);
	if (lsadcFd != -1)
	{
		for (i = 0; i < EC_LSADC_GETCNT; i++)
		{
			ioctl(lsadcFd, LSADC_IOCTL_READ, &value[i]);
			value[i] -= EC_LSADC_MIN;
		}
		//drop max and min
		for (i = 0; i < EC_LSADC_GETCNT-1; i++)
		{
			minIndex = value[minIndex] < value[i + 1] ? minIndex : i + 1;
			maxIndex = value[maxIndex] > value[i + 1] ? maxIndex : i + 1;
		}
		if (minIndex == maxIndex)
		{
			if (minIndex)
			{
				minIndex--; 
			}
			else
			{
				minIndex++;
			}
			
		}
		value[minIndex] = 0;
		value[maxIndex] = 0;
		float sum = 0;
		for (i = 0; i < EC_LSADC_GETCNT; i++)
		{
			sum += value[i];
		}
		ret = sum / (EC_LSADC_GETCNT - 2);
		
		ret = ret / EC_LSADC_BASE;
		ret = ret > 0 ? ret : 0;
        ret = ret > 1 ? 1 : ret;
	}
    close(lsadcFd);

	return ret * 100;
#else
    EC_INT value;

    ioctl(lsadcStat.lsadcFd, LSADC_IOCTL_READ, &value);

  //  dzlog_debug("current votage value %d %d", value , value * 4300 / 1024);

    return value * 4300 /1024;


#endif
    failed_0:
    return EC_FAILURE;
}

#else

#define BAT_CAP_PATH        "/sys/class/power_supply/cw-bat/capacity"
#define BAT_READ_BUF_SIZE      32
#define BAT_READ_MAX           (BAT_READ_BUF_SIZE-1)
EC_FLOAT king_lsadc_read(EC_VOID)
{
#if 1
    EC_ERR_PTR;

    EC_INT current_cap = 0;
    EC_CHAR buf[BAT_READ_BUF_SIZE] = {'\0'};
    FILE *fp = fopen (BAT_CAP_PATH, "r");
    CHECK_NULL(fp, 0);
    if (fread (buf, BAT_READ_MAX, 1, fp) != 0)
    {
        goto failed_1;
    }
    current_cap = atoi(buf);

    fclose (fp);
    return current_cap;



    failed_1:
    EC_ERR_SET("read bat failed");
    fclose (fp);
    failed_0:
    dzlog_error("open %s for check bat failed", BAT_CAP_PATH);
    return (EC_FLOAT)(-1);
#else
    return 100;
#endif
}


#endif


