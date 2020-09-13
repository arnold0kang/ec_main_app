//
// Created by arnold on 4/24/2018.
//

#ifndef EC_MAIN_APP_EC_KO_H
#define EC_MAIN_APP_EC_KO_H
#include "../common/ec_define.h"

EC_INT ec_ko_load (EC_CHAR *ko_name, EC_CHAR *param);

EC_INT ec_ko_unload (EC_CHAR *ko_name);

EC_BOOL ec_ko_is_load (EC_CHAR *ko_name);

#endif //EC_MAIN_APP_EC_KO_H
