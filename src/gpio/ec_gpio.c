//
// Created by arnold on 10/28/2018.
//

#include "ec_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>


EC_INT ec_gpio_direction(EC_INT gpio, ec_gpio_dir dir)
{
    EC_INT ret = 0;
    char buf[50];
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
    EC_INT gpiofd = open(buf, O_WRONLY);
    if (gpiofd < 0)
    {
        perror("Couldn't open IRQ file");
        return EC_FAILURE;
    }

    if (dir == EC_GPIO_HIGH && gpiofd)
    {
        if (3 != write(gpiofd, "high", 3))
        {
            perror("Couldn't set GPIO direction to out");
            return EC_FAILURE;
        }
    }

    if (dir == EC_GPIO_OUT && gpiofd)
    {
        if (3 != write(gpiofd, "out", 3))
        {
            perror("Couldn't set GPIO direction to out");
            return EC_FAILURE;
        }
    }
    else if (gpiofd)
    {
        if (2 != write(gpiofd, "in", 2))
        {
            perror("Couldn't set GPIO directio to in");
            return EC_FAILURE;
        }
    }

    close(gpiofd);
    return EC_SUCCESS;
}

EC_INT ec_gpio_setedge(EC_INT gpio, EC_BOOL rising, EC_BOOL falling)
{
    EC_INT ret = 0;
    char buf[50];
    sprintf(buf, "/sys/class/gpio/gpio%d/edge", gpio);
    EC_INT gpiofd = open(buf, O_WRONLY);
    if (gpiofd < 0)
    {
        perror("Couldn't open IRQ file");
        return EC_FAILURE;
    }

    if (gpiofd && rising && falling)
    {
        if (4 != write(gpiofd, "both", 4))
        {
            perror("Failed to set IRQ to both falling & rising");
            return EC_FAILURE;
        }
    }
    else
    {
        if (rising && gpiofd)
        {
            if (6 != write(gpiofd, "rising", 6))
            {
                perror("Failed to set IRQ to rising");
                return EC_FAILURE;
            }
        }
        else if (falling && gpiofd)
        {
            if (7 != write(gpiofd, "falling", 7))
            {
                perror("Failed to set IRQ to falling");
                return EC_FAILURE;
            }
        }
    }

    close(gpiofd);

    return EC_SUCCESS;
}

EC_INT ec_gpio_export(EC_INT gpio)
{
    EC_INT efd;
    char buf[50];
    EC_INT gpiofd, ret;

    /* Quick test if it has already been exported */
    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
    efd = open(buf, O_WRONLY);
    if (efd != -1)
    {
        close(efd);
        return EC_SUCCESS;
    }

    gpiofd = open("/sys/class/gpio/export", O_WRONLY);

    if (gpiofd != -1)
    {
        sprintf(buf, "%d", gpio);
        ret = write(gpiofd, buf, strlen(buf));
        if (ret < 0)
        {
            perror("Export failed");
            return EC_FAILURE;
        }
        close(gpiofd);
    }
    else
    {
        // If we can't open the export file, we probably
        // dont have any gpio permissions
        return EC_FAILURE;
    }
    return EC_SUCCESS;
}

void ec_gpio_unexport(EC_INT gpio)
{
    EC_INT gpiofd, ret;
    char buf[50];
    gpiofd = open("/sys/class/gpio/unexport", O_WRONLY);
    sprintf(buf, "%d", gpio);
    ret = write(gpiofd, buf, strlen(buf));
    close(gpiofd);
}

EC_INT ec_gpio_open(EC_INT gpio)
{
    char buf[50];
    EC_INT gpiofd;
    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
    gpiofd = open(buf, O_RDWR);
    if (gpiofd < 0)
    {
        return EC_FAILURE;
    }

    return gpiofd;
}


EC_VOID ec_gpio_close (EC_INT fd)
{
    close(fd);

    return;
}
EC_INT ec_gpio_read(EC_INT gpiofd)
{
    char in[3] = {0, 0, 0};
    EC_INT nread;
    lseek(gpiofd,0,SEEK_SET);
    do
    {
        nread = read(gpiofd, in, 1);
    } while (nread == 0);
    if (nread == -1)
    {
        perror("GPIO Read failed");
        return -1;
    }


    return atoi(in);
}

EC_INT ec_gpio_write(EC_INT gpiofd, EC_INT val)
{
    char buf[50];
    EC_INT ret;

    snprintf(buf, 2, "%d", val);
    ret = write(gpiofd, buf, 2);
    if (ret < 0)
    {
        return EC_FAILURE;
    }

    return EC_SUCCESS;
}
