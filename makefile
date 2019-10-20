# makefile for ffrpg project
# written by rockcarry

# ����������
CC      = gcc
AR      = ar
STRIP   = strip
CCFLAGS = -mwindows -Wall

# ���е�Ŀ���ļ�
OBJS = \
    log.o \
    bmp.o \
    screen.o \
    bitblt.o

# ���еĿ�ִ��Ŀ��
EXES = \
    bmp.exe \
    screen.exe \
    bitblt.exe

# ����Ŀ��ļ�
LIB = ffrpg.a
DLL = ffrpg.dll

# �������
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



