CC = g++
CFLAGS = -Wall -g -std=c++11

PRTC = protoc
PRTCFLAGS = -I=./proto --cpp_out=.

INCLUDE =

PROTO_FILES = $(wildcard ./proto/*.proto)           # wildcard将满足条件的内容"展开"作为变量的值
CPP_FILES = $(patsubst ./proto/%.proto, %.pb.cc, $(PROTO_FILES))
OBJ_FILES = $(patsubst ./proto/%.proto, %.pb.o, $(PROTO_FILES))

$(warning $(PROTO_FILES))
$(warning $(CPP_FILES))
$(warning $(OBJ_FILES))

TARGET_NAME = libcustproto

ifeq ($(LIB_TYPE), static)                          #ifeq关键字和左括号之间必须有个空格

all:$(TARGET_NAME).a clean

$(TARGET_NAME).a:$(OBJ_FILES)
	ar -crv $@ $^                                   #gcc.rm.ar等前面是tab分割符，不能用空格，否则会提示遗漏分隔符
	#mv $@ ../../libs

$(OBJ_FILES):$(PROTO_FILES)
	$(PRTC) $(PRTCFLAGS) $(PROTO_FILES)
	$(CC) -c $(CFLAGS) $(INCLUDE) $(CPP_FILES)

clean:
	-rm *.o                                         #加横线的目的是：当某个文件不存在时，删除操作会报错，加横线可以忽略该错误
else

all:$(TARGET_NAME).so clean

$(TARGET_NAME).so:$(OBJ_FILES)
	$(CC) -shared -fPIC -o $@ $^ -lc                #使用g++代替ld命令
	#mv $@ ../../libs

$(OBJ_FILES):$(PROTO_FILES)
	$(PRTC) $(PRTCFLAGS) $(PROTO_FILES)
	$(CC) -c $(CFLAGS) $(INCLUDE) $(CPP_FILES)

clean:
	-rm *.o
endif
