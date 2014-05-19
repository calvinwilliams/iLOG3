#ifndef _H_LOGCONF_
#define _H_LOGCONF_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.6
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifdef __cplusplus
extern "C" {
#endif

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOG.h"

/* 函数返回值宏 */
#define LOGCONF_RETURN_ERROR_CONFIGFILE_NOTFOUND	-51	/* 找不到配置文件 */
#define LOGCONF_RETURN_ERROR_CONFIGFILE_INVALID		-52	/* 配置文件无效 */

_WINDLL_FUNC LOG *CreateLogHandleFromConfig( char *config_filename , char *postfix );

#ifdef __cplusplus
}
#endif

#endif
