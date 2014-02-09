#ifndef _H_LOGCONF_
#define _H_LOGCONF_

/*
 * iLOG3 - 标准c日志函数库 简单配置文件接口
 * author  : calvin
 * email   : 
 * history : 2014-02-09	v1.0.0		创建
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
#include "LOGS.h"
#include "LOGCONF.h"

/* 函数返回值宏 */
_WINDLL_FUNC LOGS *CreateLogsHandleFromConfig( char *config_filename , char *postfix );

#ifdef __cplusplus
}
#endif

#endif
