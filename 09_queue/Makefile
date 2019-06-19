CC=gcc
DEPS=list.h
OBJ = queue.o list.o
BUILD_DIR=build

$(BUILD_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/queue: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
