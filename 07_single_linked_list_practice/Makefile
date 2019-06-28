CC=gcc
DEPS=list.h
OBJ = singly_linked_list_practice.o list.o
BUILD_DIR=build

.PHONY : clean all
all: ${BUILD_DIR}/list_practise

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p ${BUILD_DIR}
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/list_practise: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -rf *.o ${BUILD_DIR}