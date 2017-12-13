# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			LOG.c \
			LOGS.c \
			LOGCONF.c \
			LOGSCONF.c \

###### 目标文件、安装目录配置区
LIB		=	libiLOG3.so
LIBINST		=	$(HOME)/lib
LIB2		=	libiLOG3.a
# LIBINST2	=	$(HOME)/lib
HDER		= \
			LOG.h \
			LOGS.h \
			LOGCONF.h \
			LOGSCONF.h \

HDERINST	=	$(HOME)/include/iLOG3

###### 编译选项
CFLAGS		=	-I. \

###### 链接选项
LFLAGS		=	-L. \
			-lpthread \

###### 额外宏定义区
CLEAN_ADDITION	=

###### 加载mktpl模板库
#@ make_all
#@ make_clean
#@ make_install
#@ make_install_LIBINST
#@ make_install_HDERINST
#@ make_install_DFTHDERINST

include $(MKTPLDIR)/makeobj_$(MKTPLOS).inc

###### 目标文件依赖关系
libiLOG3.so		:	$(c_FILE_o)
	$(CC) -o $@ $(c_FILE_o) $(SOFLAGS) $(LFLAGS)

libiLOG3.a		:	$(c_FILE_o)
	$(AR) $(ARFLAGS) $@ $(c_FILE_o)

