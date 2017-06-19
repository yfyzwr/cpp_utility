CC = g++
CFLAGS = -Wall -g -std=c++11

PRTC = protoc
PRTCFLAGS =  -i=./proto --cpp_out=.

INCLUDE =

PROTO_FILES = $(wildcard ./proto/*.proto)
CPP_FILES = $(patsubst %.proto, %.pb.cc, $(PROTO_FILES))
OBJ_FILES = $(patsubst %.proto, %.pb.o, $(PROTO_FILES))

$(warning $(PROTO_FILES))
$(warning $(CPP_FILES))
$(warning $(OBJ_FILES))

TARGET_NAME = libcustproto

ifeq ($(LIB_TYPE), static)                          #ifeq关键字和左括号之间必须有个空格

    all:$(TARGET_NAME).a clean

    $(TARGET_NAME).a:$(OBJ_FILES)
        ar -crv $@ $<
        #mv $@ ../../libs

    %.o:%.proto
        $(PRTC) $(PRTCFLAGS) $(PROTO_FILES)
        $(CC) -c $(CFLAGS) $(INCLUDE) $(CPP_FILES)

    clean:
        -rm *.o                                     #加横线的目的是：当某个文件不存在时，删除操作会报错，加横线可以忽略该错误

endif