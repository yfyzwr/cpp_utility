CC = g++

CFLAGS = -Wall -g -std=c++11

INCLUDE = -I ./include -I /usr/local/include              #建议自定义目录放在前面

LIBS =

OBJ_FILES = file_func.o time_func.o

ifeq ($(LIB_TYPE), static)                          #ifeq关键字和左括号之间必须有个空格

LIB_SYS_RELATED = libsysrelated.a

all:$(LIB_SYS_RELATED) clean

$(LIB_SYS_RELATED):$(OBJ_FILES)
        ar -crv $(LIB_SYS_RELATED) $(OBJ_FILES)
        mv $@ ../../libs

%.o:%.cpp
        $(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
        -rm *.o                                     #加横线的目的是：当某个文件不存在时，删除操作会报错，加横线可以忽略该错误

else

LIB_SYS_RELATED = libsysrelated.so

all:$(LIB_SYS_RELATED) clean

$(LIB_SYS_RELATED):$(OBJ_FILES)
        $(CC) -shared -fPIC -o $(LIB_SYS_RELATED) $(OBJ_FILES) -lc              #使用g++代替ld命令
        mv $@ ../../libs

%.o:%.cpp
        $(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
#       -rm *.o

endif