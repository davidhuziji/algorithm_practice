CC=gcc
OBJ = rk_substr_search.o
BUILD_DIR=build
CFLAGS=-lm

.PHONY : clean all
all: ${BUILD_DIR}/rk_substr_search

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p ${BUILD_DIR}
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/rk_substr_search: $(BUILD_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -rf *.o ${BUILD_DIR}