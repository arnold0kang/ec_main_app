//
// Created by arnold on 10/28/2018.
//

#ifndef EC_MAIN_APP_EC_GPIO_H
#define EC_MAIN_APP_EC_GPIO_H

#include "../common/ec_define.h"

typedef enum {
    EC_GPIO_NONE,
    EC_GPIO_IN,
    EC_GPIO_OUT,
    EC_GPIO_HIGH
}ec_gpio_dir;

/* returns -1 or the file descriptor of the gpio value file */
EC_INT ec_gpio_export(EC_INT gpio);
/* Set direction to 2 = high output, 1 low output, 0 input */
EC_INT ec_gpio_direction(EC_INT gpio, ec_gpio_dir dir);
/* Release the GPIO to be claimed by other processes or a kernel driver */
EC_VOID ec_gpio_unexport(EC_INT gpio);
/* Single GPIO read */
EC_INT ec_gpio_read(EC_INT fd);
/* Set GPIO to val (1 = high) */
EC_INT ec_gpio_write(EC_INT fd, EC_INT val);
/* Set which edge(s) causes the value select to return */
EC_INT ec_gpio_setedge(EC_INT gpio, EC_BOOL rising, EC_BOOL falling);

EC_INT ec_gpio_open(EC_INT gpio);

EC_VOID ec_gpio_close (EC_INT fd);


#endif //EC_MAIN_APP_EC_GPIO_H
