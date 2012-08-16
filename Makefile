CC = gcc
ASM = nasm
DBG = gdb
CFLAGS = -ggdb -Wall -Wextra  -std=c99 -pedantic
ASMFLAGS = -felf32 -g -F dwarf
OPENCV_LIB = `pkg-config --cflags --libs opencv`

BIN = opencv

OBJS = opencv.o \
		roberts_c.o prewitt_c.o sobel_c.o freichen_c.o \
		utils.o

.PHONY: all debug test clean

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(OPENCV_LIB)

opencv.o: opencv.c
	$(CC) $(CFLAGS) -c -o $@ $< $(OPENCV_LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
		
all: $(BIN)
		
clean:
	rm -f $(OBJS)
	rm -f ./$(BIN)
	rm -f $(BIN_DIR)/$(BIN)
