CC = gcc
ASM = nasm
DBG = gdb
CFLAGS = -ggdb -Wall -Wextra  -std=c99 -pedantic
ASMFLAGS = -felf64 -g -F dwarf
OPENCV_LIB = `pkg-config --cflags --libs opencv`

BIN = opencv

OBJS = opencv.o \
		listafiltro.o\
		filtros/c/rotar_c.o\
		filtros/asm/rotar_asm.o\
		filtros/c/scale2x_c.o\
		filtros/asm/scale2x_asm.o\
		filtros/asm/monocromatizar_asm.o\
		filtros/c/monocromatizar_c.o\
		filtros/asm/sepia_asm.o\
		filtros/c/sepia_c.o\
		filtros/asm/smalltiles_asm.o\
		filtros/c/smalltiles_c.o\
		filtros/asm/blur_asm.o\
		filtros/c/blur_c.o\
        filtros/asm/invertir_asm.o\
        filtros/c/invertir_c.o\
        filtros/c/blit_c.o\
        filtros/asm/blit_asm.o\
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
