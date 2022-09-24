TARGET = mp3play_lite
OBJS = main.o mp3.o file.o memory.o draw.o conf.o sysclib.o utils.o stub_list.o exports.o

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1
PRX_EXPORTS	= exports.exp

BUILD_PRX = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 

LIBS = -lpspkernel -lc -lpspuser -lpspsystemctrl_kernel -lpspaudio -lpspaudiocodec -lpsputility

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

