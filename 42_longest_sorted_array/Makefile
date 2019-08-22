CC=gcc
OBJ=longest_sorted_array.o
BUILD_DIR=build
CFLAGS=

.PHONY : clean all
all: ${BUILD_DIR}/long_sort

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p ${BUILD_DIR}
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/long_sort: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -rf *.o ${BUILD_DIR}
