CC=gcc
OBJ = merge_quick_sort.o
BUILD_DIR=build

$(BUILD_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sort: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
