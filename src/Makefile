CC = gcc
CFLAGS = -Wall -Wextra -Werror -I. -Ifunctions/
CFLAGS_GCOV = $(CFLAGS) -fprofile-arcs -ftest-coverage

LDFLAGS_MACOS = -lcheck -lm -lpthread
LDFLAGS_LINUX = -lcheck -lm -lpthread -lsubunit
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
    LDFLAGS = $(LDFLAGS_MACOS)
else
    LDFLAGS = $(LDFLAGS_LINUX)
	CFLAGS += -std=c11
endif

OBJ_DIR = obj
SRC_DIR = functions
ARITHMETIC_DIR = $(SRC_DIR)/arithmetic_operators
COMPARISON_DIR = $(SRC_DIR)/comparison
CONVERTERS_DIR = $(SRC_DIR)/converters
HELPERS_DIR = $(SRC_DIR)/helpers
OTHER_DIR = $(SRC_DIR)/other
TEST_SRC = s21_test.c

SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(ARITHMETIC_DIR)/*.c) \
      $(wildcard $(COMPARISON_DIR)/*.c) \
      $(wildcard $(CONVERTERS_DIR)/*.c) \
      $(wildcard $(HELPERS_DIR)/*.c) \
      $(wildcard $(OTHER_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c)) \
      $(patsubst $(ARITHMETIC_DIR)/%.c,$(OBJ_DIR)/arithmetic_operators/%.o,$(wildcard $(ARITHMETIC_DIR)/*.c)) \
      $(patsubst $(COMPARISON_DIR)/%.c,$(OBJ_DIR)/comparison/%.o,$(wildcard $(COMPARISON_DIR)/*.c)) \
      $(patsubst $(CONVERTERS_DIR)/%.c,$(OBJ_DIR)/converters/%.o,$(wildcard $(CONVERTERS_DIR)/*.c)) \
      $(patsubst $(HELPERS_DIR)/%.c,$(OBJ_DIR)/helpers/%.o,$(wildcard $(HELPERS_DIR)/*.c)) \
      $(patsubst $(OTHER_DIR)/%.c,$(OBJ_DIR)/other/%.o,$(wildcard $(OTHER_DIR)/*.c))

GCOV_OBJS = $(OBJ:.o=_gcov.o)

TEST_EXEC = s21_test
GCOV_EXEC = s21_test_gcov
LIBRARY = s21_decimal.a
REPORT_DIR = report

all: $(TEST_EXEC)

$(shell mkdir -p $(OBJ_DIR)/arithmetic_operators)
$(shell mkdir -p $(OBJ_DIR)/comparison)
$(shell mkdir -p $(OBJ_DIR)/converters)
$(shell mkdir -p $(OBJ_DIR)/helpers)
$(shell mkdir -p $(OBJ_DIR)/other)

$(TEST_EXEC): $(OBJ) $(TEST_SRC)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRC) $(OBJ) $(LDFLAGS)

$(GCOV_EXEC): $(GCOV_OBJS) $(TEST_SRC)
	$(CC) $(CFLAGS_GCOV) -o $@ $(TEST_SRC) $(GCOV_OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/arithmetic_operators/%.o: $(ARITHMETIC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/comparison/%.o: $(COMPARISON_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/converters/%.o: $(CONVERTERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/helpers/%.o: $(HELPERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/other/%.o: $(OTHER_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_gcov.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(OBJ_DIR)/arithmetic_operators/%_gcov.o: $(ARITHMETIC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(OBJ_DIR)/comparison/%_gcov.o: $(COMPARISON_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(OBJ_DIR)/converters/%_gcov.o: $(CONVERTERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(OBJ_DIR)/helpers/%_gcov.o: $(HELPERS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(OBJ_DIR)/other/%_gcov.o: $(OTHER_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_GCOV) -c -o $@ $<

$(LIBRARY): $(OBJ)
	ar rcs $@ $(OBJ)
	ranlib $@
	@echo "Static library $@ successfully built"

test: $(TEST_EXEC)
	./$(TEST_EXEC)

gcov_report: $(GCOV_EXEC)
	@mkdir -p $(REPORT_DIR)
	./$(GCOV_EXEC)
	lcov -t "gcov_report" -o $(REPORT_DIR)/coverage.info -c -d $(OBJ_DIR)
	genhtml -o $(REPORT_DIR) $(REPORT_DIR)/coverage.info
	@echo "Open $(REPORT_DIR)/index.html in your browser"

clang-format:
	find . -name '*.c' -o -name '*.h' | xargs clang-format -i --style=Google

clean:
	rm -rf $(OBJ_DIR) $(REPORT_DIR) $(TEST_EXEC) $(GCOV_EXEC) $(LIBRARY) *.gcda *.gcno

rebuild: clean all