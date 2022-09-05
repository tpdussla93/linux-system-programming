OUT_DIR = out/
OBJ_DIR = obj/

TARGET = minils
CC := gcc -Wall
CC := $(CC) -g
CFLAGS = -I. -c 
LFLAGS =


OBJS = $(OBJ_DIR)/$(TARGET).o $(OBJ_DIR)/argparse.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/ls.o

all : $(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(OUT_DIR)
	$(CC) $(LFLAGS) $(OBJ_DIR)/*.o -o $@

$(OBJ_DIR)/argparse.o : src/argparse.c src/argparse.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/utils.o : src/utils.c src/utils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/ls.o : src/ls.c src/ls.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/$(TARGET).o: src/minils.c src/argparse.h src/utils.h src/ls.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf out obj