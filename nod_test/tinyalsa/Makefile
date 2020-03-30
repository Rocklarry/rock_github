CC = gcc
CXXFLAGS = 
INC_DIR = ./include
LIB_DIR = ./lib
OBJ_DIR = ./obj
APP_DIR = ./app

TARGET = $(LIB_DIR)/libtinyalsa.so


CFLAGS = -Wall -O -g -fPIC -I$(INC_DIR)

SRCS = $(wildcard $(APP_DIR)/*.c)  
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

$(TARGET):$(OBJS)
	$(CC) -shared -fPIC -o $@ $(OBJS)

# app module
$(OBJ_DIR)/%.o:$(APP_DIR)/%.c 
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<


clean:
	rm -f $(OBJS) $(LIBPATH)$(TARGET)


gcc tinycap.c -I ./../include/  -L ./../lib/mylib.so  -o tinycap
