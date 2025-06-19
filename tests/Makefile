CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LDFLAGS = 

# Valgrind configuration
VALGRIND = valgrind
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=1

# Source files from parent directory
LIB_SOURCES = ../arena.c ../csv_config.c ../csv_utils.c ../csv_parser.c ../csv_writer.c ../csv_reader.c

# Test executables
TESTS = test_arena test_csv_config test_csv_utils test_csv_parser test_csv_writer test_csv_reader
TEST_RUNNER = run_all_tests

.PHONY: all clean test help valgrind valgrind-all valgrind-arena valgrind-config valgrind-utils valgrind-parser valgrind-writer valgrind-reader

all: $(TESTS) $(TEST_RUNNER)

# Individual test targets
test_arena: test_arena.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_csv_config: test_csv_config.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_csv_utils: test_csv_utils.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_csv_parser: test_csv_parser.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_csv_writer: test_csv_writer.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_csv_reader: test_csv_reader.c $(LIB_SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Test runner
$(TEST_RUNNER): run_all_tests.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run all tests
test: all
	./$(TEST_RUNNER)

# Run individual test suites
test-arena: test_arena
	./test_arena

test-config: test_csv_config
	./test_csv_config

test-utils: test_csv_utils
	./test_csv_utils

test-parser: test_csv_parser
	./test_csv_parser

test-writer: test_csv_writer
	./test_csv_writer

test-reader: test_csv_reader
	./test_csv_reader

# Valgrind targets
valgrind: valgrind-all

valgrind-all: all
	@echo "🔍 Running all tests under Valgrind..."
	@echo "======================================"
	@for test in $(TESTS); do \
		echo "🧪 Running $$test under Valgrind..."; \
		$(VALGRIND) $(VALGRIND_FLAGS) ./$$test; \
		if [ $$? -eq 0 ]; then \
			echo "✅ $$test passed Valgrind check"; \
		else \
			echo "❌ $$test failed Valgrind check"; \
			exit 1; \
		fi; \
		echo ""; \
	done
	@echo "🎉 All tests passed Valgrind checks!"

valgrind-arena: test_arena
	@echo "🔍 Running arena tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_arena

valgrind-config: test_csv_config
	@echo "🔍 Running CSV config tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_csv_config

valgrind-utils: test_csv_utils
	@echo "🔍 Running CSV utils tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_csv_utils

valgrind-parser: test_csv_parser
	@echo "🔍 Running CSV parser tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_csv_parser

valgrind-writer: test_csv_writer
	@echo "🔍 Running CSV writer tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_csv_writer

valgrind-reader: test_csv_reader
	@echo "🔍 Running CSV reader tests under Valgrind..."
	$(VALGRIND) $(VALGRIND_FLAGS) ./test_csv_reader

# Clean up
clean:
	rm -f $(TESTS) $(TEST_RUNNER)
	rm -f *.csv

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all test executables"
	@echo "  test         - Build and run all tests"
	@echo "  test-arena   - Run only arena tests"
	@echo "  test-config  - Run only CSV config tests"
	@echo "  test-utils   - Run only CSV utils tests"
	@echo "  test-parser  - Run only CSV parser tests"
	@echo "  test-writer  - Run only CSV writer tests"
	@echo "  test-reader  - Run only CSV reader tests"
	@echo ""
	@echo "Valgrind targets:"
	@echo "  valgrind         - Run all tests under valgrind"
	@echo "  valgrind-all     - Run all tests under valgrind"
	@echo "  valgrind-arena   - Run arena tests under valgrind"
	@echo "  valgrind-config  - Run config tests under valgrind"
	@echo "  valgrind-utils   - Run utils tests under valgrind"
	@echo "  valgrind-parser  - Run parser tests under valgrind"
	@echo "  valgrind-writer  - Run writer tests under valgrind"
	@echo "  valgrind-reader  - Run reader tests under valgrind"
	@echo ""
	@echo "  clean        - Remove all test executables and temporary files"
	@echo "  help         - Show this help message" 