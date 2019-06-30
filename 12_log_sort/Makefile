CC=gcc
OBJ = log_sort.o
BUILD_DIR=build

.PHONY : clean all
all: ${BUILD_DIR}/log_sort

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p ${BUILD_DIR}
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sort: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -rf *.o ${BUILD_DIR}