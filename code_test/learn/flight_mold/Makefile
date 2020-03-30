#设置编译器 
CC = gcc

#读取的文件夹
SUBDIRS = flight main obj

OBJS = flight.o main.o

#最终bin文件的名字，可以更改为自己需要的 
BIN = flight

#目标文件所在目录
OBJS_DIR = obj

#bin文件所在的目录  
BIN_DIR = myflight

#记住当前工程的根目录路径ROOT_DIR=$(shell pwd)  
#获取当前目录下的c文件集，放在变量CUR_SOURCE中CUR_SOURCE=${wildcard *.c}  
#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}  

#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中  
export CC OBJS BIN OBJS_DIR BIN_DIR


#注意这里的顺序，需要先执行CHECK_DIR最后才能是SUBDIRS  
all:CHECK_DIR $(SUBDIRS)

CHECK_DIR:
#mkdir -p 可以一次生成多级目录  
	mkdir -p $(BIN_DIR)

#递归执行子目录下的makefile文件，这是递归执行的关键 
$(SUBDIRS):ECHO 
	#直接去根目录下的其他目录下执行子makefile文件  
	make -C $@

ECHO:
	 #将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR  
	@echo $(SUBDIRS)
	@echo begin compile

#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR  
#$(CUR_OBJS):%.o:%.c  
#    $(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@  

clean:
	@$(RM) $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)
