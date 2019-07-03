CC=gcc
OBJ = sqrt.o
BUILD_DIR=build

.PHONY : clean all
all: ${BUILD_DIR}/sqrt

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p ${BUILD_DIR}
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sqrt: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -rf *.o ${BUILD_DIR}
