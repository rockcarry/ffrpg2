# makefile for ffrpg project
# written by rockcarry

# 编译器定义
CC      = gcc
AR      = ar
STRIP   = strip
CCFLAGS = -mwindows -Wall

# 所有的目标文件
OBJS = \
    log.o \
    bmp.o \
    screen.o \
    bitblt.o

# 所有的可执行目标
EXES = \
    bmp.exe \
    screen.exe \
    bitblt.exe

# 输出的库文件
LIB = ffrpg.a
DLL = ffrpg.dll

# 编译规则
all : $(LIB) $(DLL) $(EXES)

$(LIB) : $(OBJS)
	$(AR) -rv $@ $(OBJS)

$(DLL) : $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $(OBJS) --shared
	$(STRIP) $@

%.o : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.c stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.exe : %.c %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL) -D_TEST_
	$(STRIP) $@

%.exe : %.cpp %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL) -D_TEST_
	$(STRIP) $@

clean :
	-rm -f *.o
	-rm -f *.dll
	-rm -f *.exe
	-rm -f *.log

# rockcarry
# 2018.6.12
# THE END



