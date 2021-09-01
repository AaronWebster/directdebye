# Makefile

TARGET = findmax

CC = gcc

CFLAGS =

INCLUDE_PATH = -I$(HOME)/include
LIBRARY_PATH = -L$(HOME)/lib 

LIBS = -lnlopt `pkg-config --libs gsl` -lm

OBJS = findmax.o tsvread.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBRARY_PATH) $(LIBS) $(CFLAGS)

.c.o:
	$(CC) -c $*.c $(CFLAGS) $(INCLUDE_PATH)

clean:
	rm -f $(OBJS) $(TARGET)

