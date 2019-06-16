CC=gcc
DEPS=list.h
OBJ = singly_linked_list_practice.o list.o
BUILD_DIR=build

$(BUILD_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/list_practice: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
