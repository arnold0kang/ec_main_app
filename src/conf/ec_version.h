//
// Created by arnold on 17-12-8.
//

#ifndef EC_MAIN_APP_EC_VERSION_H
#define EC_MAIN_APP_EC_VERSION_H

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define APP_NAME		"enforceCam"
#define MAJOR_VERSION    1
#define MINOR_VERSION	 2

#ifndef BUILD_TYPE
#define BUILD_TYPE		release
#endif
#ifndef BUILD_TIME
#define BUILD_TIME		nulltime
#endif



#define VERSION_STR		APP_NAME "-" STR(BUILD_TYPE) "-" STR(MAJOR_VERSION) "." STR(MINOR_VERSION) "-" STR(BUILD_TIME)

#endif //EC_MAIN_APP_EC_VERSION_H
