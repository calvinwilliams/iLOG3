#ifndef _H_LOGCONF_KVFILE_
#define _H_LOGCONF_KVFILE_

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

/* 读取配置文件，创建日志句柄 */ /* create log handle from config file */
_WINDLL_FUNC LOG *CreateLogHandleFromKvFile( char *config_filename , char *postfix );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
_WINDLL_FUNC LOG *CreateLogHandleFromKvFileG( char *config_filename , char *file_ext_name );
#endif

/* 风格替换宏 */
#define create_log_handle_from_kvfile		CreateLogHandleFromKvFile

#define create_log_handle_from_kvfile_g		CreateLogHandleFromKvFileG

/* 内部函数 */
_WINDLL_FUNC int ReadKvFileForLogHandle( LOG *g , FILE *fp , char *id , int id_bufsize );

#ifdef __cplusplus
}
#endif

#endif
