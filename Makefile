# Makefile

TARGET = directdebye

CC = mpicc

CFLAGS = -O3

INCLUDE_PATH = -I$(WOODYHOME)/include
LIBRARY_PATH = -L$(WOODYHOME)/lib 

LIBS = -lm -lrt -lhdf5 -lz -lmatheval -lgsl -lgslcblas -lm

OBJS = directdebye.o rays.o hdfio.o miscprint.o parseargs.o tsvread.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBRARY_PATH) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c $(INCLUDE_PATH) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

