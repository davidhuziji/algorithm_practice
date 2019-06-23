CC=gcc
OBJ = eight_queens_puzzles.o
BUILD_DIR=build

$(BUILD_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/queen: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
