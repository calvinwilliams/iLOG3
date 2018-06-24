# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			test_logconf_kvfile.c \
			test_logsconf_kvfile.c \

###### 目标文件、安装目录配置区
BIN		=	$(c_FILE:.c=)

###### 编译选项
CFLAGS		=	-I. \
			-I../src \
			-I../src-conf-kvfile \
			-std=gnu99 \

###### 链接选项
LFLAGS		=	-L. \
			-L../src-conf-kvfile \
			-liLOG3CONF_KVFILE \
			-L../src \
			-liLOG3 \
			-lpthread \

###### 额外宏定义区
CLEAN_ADDITION	=

###### 加载mktpl模板库
#@ make_all
#@ make_clean
#@ make_install
#@ make_install_BININST
#@ make_install_DFTHDERINST

include $(MKTPL2_HOME)/makeobj_$(MKTPL2_OS).inc

###### 目标文件依赖关系
test_logconf_kvfile		:	test_logconf_kvfile.o
	$(CC) -o $@ test_logconf_kvfile.o $(LFLAGS)

test_logsconf_kvfile		:	test_logsconf_kvfile.o
	$(CC) -o $@ test_logsconf_kvfile.o $(LFLAGS)

