CC=gcc
DEPS=palindrome_list.h
OBJ = palindrome.o palindrome_list.o
BUILD_DIR=build

$(BUILD_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/palindrome: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
