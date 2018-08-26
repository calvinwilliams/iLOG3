#ifndef _H_LOGSCONF_KVFILE_
#define _H_LOGSCONF_KVFILE_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "LOG.h"
#include "LOGS.h"

#include "LOGCONF_KVFILE.h"

/* 读取配置文件，创建日志句柄集合 */ /* create log handles from config file */
_WINDLL_FUNC LOGS *CreateLogsHandleFromKvFile( char *config_filename , char *file_ext_name );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
_WINDLL_FUNC LOGS *CreateLogsHandleFromKvFileG( char *config_filename , char *file_ext_name );
#endif

/* 风格替换宏 */
#define create_logs_handle_from_config		CreateLogsHandleFromConfig

#define create_logs_handle_from_kvfile_g	CreateLogsHandleFromKvFileG

#ifdef __cplusplus
}
#endif

#endif
