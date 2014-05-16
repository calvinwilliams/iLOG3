# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			test_hello.c \
			test_demo.c \
			test_stylesfunc.c \
			test_outputfunc.c \
			test_press.c \
			test_press_mpt.c \
			test_press_mpt_tls.c \
			test_memoryleak.c \
			test_funny.c \
			test_logconf.c \
			test_logsconf.c \
			test_afterrotatefile.c \
			test_filterlogfunc.c \
			bench_tiny.c \

###### 目标文件、安装目录配置区
BIN		=	$(c_FILE:.c=)

###### 编译选项
CFLAGS		=	-I. \
			-I../src \

###### 链接选项
LFLAGS		=	-L. \
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

include $(MKTPLDIR)/makeobj_$(MKTPLOS).inc

###### 目标文件依赖关系
test_hello		:	test_hello.o
	$(CC) -o $@ test_hello.o $(LFLAGS)

test_demo		:	test_demo.o
	$(CC) -o $@ test_demo.o $(LFLAGS)

test_press		:	test_press.o
	$(CC) -o $@ test_press.o $(LFLAGS)

test_press_mpt		:	test_press_mpt.o
	$(CC) -o $@ test_press_mpt.o $(LFLAGS)

test_press_mpt_tls	:	test_press_mpt_tls.o
	$(CC) -o $@ test_press_mpt_tls.o $(LFLAGS)

test_stylesfunc		:	test_stylesfunc.o
	$(CC) -o $@ test_stylesfunc.o $(LFLAGS)

test_outputfunc		:	test_outputfunc.o
	$(CC) -o $@ test_outputfunc.o $(LFLAGS)

test_memoryleak		:	test_memoryleak.o
	$(CC) -o $@ test_memoryleak.o $(LFLAGS)

test_funny		:	test_funny.o
	$(CC) -o $@ test_funny.o $(LFLAGS)

test_logconf		:	test_logconf.o
	$(CC) -o $@ test_logconf.o $(LFLAGS)

test_logsconf		:	test_logsconf.o
	$(CC) -o $@ test_logsconf.o $(LFLAGS)

test_afterrotatefile	:	test_afterrotatefile.o
	$(CC) -o $@ test_afterrotatefile.o $(LFLAGS)

test_filterlogfunc	:	test_filterlogfunc.o
	$(CC) -o $@ test_filterlogfunc.o $(LFLAGS)

bench_tiny		:	bench_tiny.o
	$(CC) -o $@ bench_tiny.o $(LFLAGS)

