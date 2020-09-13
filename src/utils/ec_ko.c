//
// Created by arnold on 4/24/2018.
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "ec_ko.h"
#include "../log/ec_log.h"

//default for uclibc , if use glic , this not work, should use syscall
extern  int init_module(void *module_image, unsigned long len,
                const char *param_values);

extern int delete_module(const char *name, int flags);



EC_INT ec_ko_load (EC_CHAR *ko_name, EC_CHAR *param)
{
    int fd = open (ko_name, O_RDONLY);
    if (-1 == fd)
    {
        dzlog_error("open module %s failed\n", ko_name);
        return EC_FAILURE;
    }
    struct stat statBuf;
    memset(&statBuf, 0, sizeof (statBuf));
    fstat(fd, &statBuf);
    void *image = malloc(statBuf.st_size);
    if (image == NULL)
    {
        dzlog_error("malloc fialed");
        close (fd);
        return EC_FAILURE;
    }
    if (read (fd, image, statBuf.st_size)!= statBuf.st_size)
    {
        dzlog_error("read module %s  size %dfailed", ko_name, statBuf.st_size);
        free(image);
        close (fd);
        return EC_FAILURE;
    }
    if (init_module(image, statBuf.st_size, param == EC_NULL ? " " : param) ==-1)
    {
        if (errno != EEXIST)
        {
            perror ("init mododule:");
            return EC_FAILURE;
        }

    }

    free (image);
    close (fd);

    return EC_SUCCESS;

}

EC_INT ec_ko_unload (EC_CHAR *ko_name)
{
    dzlog_debug("unload %s", ko_name);
    if (delete_module(ko_name, O_NONBLOCK | O_TRUNC) == -1)
    {
        if (errno != ENOENT)
        {
            dzlog_error("unload module %s failed\n", ko_name);
            return EC_FAILURE;
        }

    }
    return EC_SUCCESS;
}

EC_BOOL ec_ko_is_load (EC_CHAR *ko_name)
{

}