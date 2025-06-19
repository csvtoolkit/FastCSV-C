# CSV Library Test Suite

This directory contains comprehensive tests for all modules of the CSV library.

## Test Files

- **`test_arena.c`** - Tests for arena memory management (12 functions)
- **`test_csv_config.c`** - Tests for CSV configuration management (14 functions)  
- **`test_csv_utils.c`** - Tests for CSV utility functions (6 functions)
- **`test_csv_parser.c`** - Tests for CSV parsing functions (6 functions)
- **`test_csv_writer.c`** - Tests for CSV writing functions (11 functions)
- **`test_csv_reader.c`** - Tests for CSV reading functions (10 functions)
- **`run_all_tests.c`** - Master test runner that executes all test suites

## Building and Running Tests

### Prerequisites
- GCC compiler
- Make utility
- POSIX-compliant system (for fork/exec in test runner)

### Quick Start
```bash
# Build and run all tests
make test

# Or step by step:
make all        # Build all test executables
./run_all_tests # Run all tests with summary
```

### Individual Test Suites
```bash
# Run specific test suites
make test-arena     # Arena memory management tests
make test-config    # CSV configuration tests  
make test-utils     # CSV utility function tests
make test-parser    # CSV parsing tests
make test-writer    # CSV writing tests
make test-reader    # CSV reading tests
```

### Manual Execution
```bash
# Build individual tests
make test_arena
./test_arena

# Or compile manually
gcc -Wall -Wextra -std=c99 -I.. -o test_arena test_arena.c ../arena.c ../csv_config.c ../csv_utils.c ../csv_parser.c ../csv_writer.c ../csv_reader.c
```

## Test Coverage

### Arena Tests (12 tests)
- ✅ Basic arena creation and destruction
- ✅ Arena allocation with alignment
- ✅ Out of memory handling
- ✅ Arena reset and regions
- ✅ String duplication
- ✅ Size tracking functions
- ✅ Null pointer safety
- ✅ Buffer-based arena creation

### CSV Config Tests (9 tests)
- ✅ Configuration creation and copying
- ✅ All getter/setter functions
- ✅ Path management
- ✅ Null pointer safety
- ✅ Default value validation

### CSV Utils Tests (11 tests)
- ✅ Whitespace detection and trimming
- ✅ CSV character validation
- ✅ Field escaping detection
- ✅ Buffer overflow protection
- ✅ Error handling and reporting

### CSV Parser Tests (13 tests)
- ✅ Simple CSV line parsing
- ✅ Quoted field handling
- ✅ Escaped quote processing
- ✅ Empty field handling
- ✅ Custom delimiter support
- ✅ Header parsing
- ✅ Multiline record reading
- ✅ Field counting
- ✅ Generic parsing functions

### CSV Writer Tests (11 tests)
- ✅ Writer initialization
- ✅ Record writing with headers
- ✅ Automatic field quoting
- ✅ Custom delimiters and enclosures
- ✅ Map-based record writing
- ✅ File and stream handling
- ✅ Error handling

### CSV Reader Tests (12 tests)
- ✅ Reader initialization
- ✅ Record iteration
- ✅ Header processing
- ✅ Field access by name
- ✅ Custom delimiters
- ✅ Quoted field parsing
- ✅ File offset handling
- ✅ Position tracking
- ✅ End-of-file detection

## Test Output

### Successful Run
```
🚀 CSV Library Test Suite Runner
================================

🧪 Running Arena Tests...
========================================
Testing arena_create...
✓ arena_create passed
...
✅ All Arena tests passed!
✅ Arena Tests PASSED

...

📊 Test Results Summary
========================================
Total Test Suites: 6
✅ Passed: 6
❌ Failed: 0

🎉 All tests passed! Your CSV library is working correctly.
```

### Failed Test Example
```
❌ CSV Writer Tests FAILED

📊 Test Results Summary
========================================
Total Test Suites: 6
✅ Passed: 5
❌ Failed: 1

💥 Some tests failed. Please check the output above.
```

## Cleanup
```bash
make clean  # Remove all test executables and temporary files
```

## Adding New Tests

1. Create a new test file: `test_new_module.c`
2. Follow the existing pattern:
   ```c
   #include <stdio.h>
   #include <assert.h>
   #include "../your_module.h"
   
   void test_function_name() {
       printf("Testing function_name...\n");
       // Your test code
       assert(condition);
       printf("✓ function_name test passed\n");
   }
   
   int main() {
       printf("Running New Module Tests...\n\n");
       test_function_name();
       printf("\n✅ All New Module tests passed!\n");
       return 0;
   }
   ```
3. Add to `Makefile` and `run_all_tests.c`

## Notes

- Tests use `assert()` for validation - failed assertions will terminate the test
- Temporary files are created and cleaned up automatically
- Each test suite runs in isolation via fork/exec
- All tests should pass on a properly functioning CSV library
- Tests cover both success and error conditions 