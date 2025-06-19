# CSV Library

[![Build Status](https://github.com/csvtoolkit/FastCSV-C/workflows/CI/badge.svg)](https://github.com/csvtoolkit/FastCSV-C/actions)
[![Memory Safe](https://img.shields.io/badge/memory-safe-brightgreen.svg)](https://github.com/csvtoolkit/FastCSV-C)
[![Tests](https://img.shields.io/badge/tests-60%2B%20passing-brightgreen.svg)](https://github.com/csvtoolkit/FastCSV-C)
[![Valgrind](https://img.shields.io/badge/valgrind-clean-brightgreen.svg)](https://github.com/csvtoolkit/FastCSV-C)
[![Performance](https://img.shields.io/badge/performance-7.6M%20ops%2Fsec-blue.svg)](https://github.com/csvtoolkit/FastCSV-C)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C99](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)

A high-performance, memory-safe CSV parsing and writing library written in C with custom arena-based memory management. Designed for production use with zero memory leaks, comprehensive error handling, and enterprise-grade features including multi-encoding support and RFC 4180 compliance.

## 🚀 Features

- **🛡️ Memory Safe**: Zero memory leaks, validated with Valgrind
- **⚡ Ultra High Performance**: 7.6M+ operations/second with optimized parsing
- **🎯 Custom Memory Management**: Arena-based allocator for efficient memory usage
- **🌐 Multi-Encoding Support**: UTF-8, UTF-16, UTF-32, ASCII, Latin1 with BOM support
- **📝 RFC 4180 Compliant**: Proper quote escaping and multi-line field support
- **🔧 Flexible Configuration**: Customizable delimiters, quotes, strict mode, and field trimming
- **📊 Advanced Reader Features**: Navigation, seeking, header management, and position tracking
- **✅ Comprehensive Testing**: 60+ tests across 6 test suites with 100% pass rate
- **🌐 Cross-Platform**: Works on Linux, macOS, and other Unix-like systems
- **📚 Library Ready**: Designed for integration into larger projects and language bindings

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Encoding Support](#encoding-support)
- [Advanced Features](#advanced-features)
- [Testing](#testing)
- [Performance](#performance)
- [Memory Safety](#memory-safety)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## 🔧 Installation

### Prerequisites

- C99 compatible compiler (GCC, Clang)
- POSIX-compliant system
- Make build system
- Valgrind (optional, for memory testing)

### Build from Source

```bash
git clone https://github.com/csvtoolkit/FastCSV-C.git
cd FastCSV-C

# Build shared and static libraries
make

# Run tests to verify installation
make test

# Optional: Run memory safety checks
make valgrind

# Performance benchmarks
make benchmark
```

### Build Targets

| Target | Description |
|--------|-------------|
| `make` | Build shared and static libraries |
| `make shared` | Build shared library (`libcsv.so`) |
| `make static` | Build static library (`libcsv.a`) |
| `make test` | Run all tests |
| `make valgrind` | Run tests with Valgrind |
| `make benchmark` | Run performance benchmarks |
| `make clean` | Clean build artifacts |
| `make help` | Show all available targets |

## 🚀 Quick Start

### Reading CSV Files

```c
#include "csv_reader.h"
#include "arena.h"

int main() {
    // Initialize arena allocator
    Arena arena;
    arena_create(&arena, 4096);
    
    // Create configuration with encoding support
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "data.csv");
    csv_config_set_has_header(config, true);
    csv_config_set_encoding(config, CSV_ENCODING_UTF8);
    
    // Initialize reader
    CSVReader *reader = csv_reader_init_with_config(&arena, config);
    
    // Get headers
    int header_count;
    char **headers = csv_reader_get_headers(reader, &header_count);
    printf("Headers: ");
    for (int i = 0; i < header_count; i++) {
        printf("%s ", headers[i]);
    }
    printf("\n");
    
    // Read records with navigation support
    while (csv_reader_has_next(reader)) {
        CSVRecord *record = csv_reader_next_record(reader);
        if (record) {
            printf("Record at position %ld:\n", csv_reader_get_position(reader));
            for (int i = 0; i < record->field_count; i++) {
                printf("  %s: %s\n", headers[i], record->fields[i]);
            }
        }
    }
    
    // Cleanup
    csv_reader_free(reader);
    arena_destroy(&arena);
    return 0;
}
```

### Writing CSV Files with Encoding

```c
#include "csv_writer.h"
#include "arena.h"

int main() {
    Arena arena;
    arena_create(&arena, 4096);
    
    // Configure with UTF-8 and BOM
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "output.csv");
    csv_config_set_encoding(config, CSV_ENCODING_UTF8);
    csv_config_set_write_bom(config, true);
    csv_config_set_strict_mode(config, true);
    
    // Initialize writer
    CSVWriter *writer;
    char *headers[] = {"Name", "Age", "City"};
    csv_writer_init(&writer, config, headers, 3, &arena);
    
    // Write data with automatic quoting
    char *row1[] = {"John Doe", "30", "New York"};
    csv_writer_write_record(writer, row1, 3);
    
    char *row2[] = {"Jane Smith", "25", "Los Angeles"};
    csv_writer_write_record(writer, row2, 3);
    
    csv_writer_free(writer);
    arena_destroy(&arena);
    return 0;
}
```

## 📖 API Reference

### Core Components

| Component | Description |
|-----------|-------------|
| **Arena** (`arena.h`) | Custom memory allocator |
| **CSV Parser** (`csv_parser.h`) | Low-level parsing engine with RFC 4180 support |
| **CSV Reader** (`csv_reader.h`) | High-level reading interface with navigation |
| **CSV Writer** (`csv_writer.h`) | CSV output generation with encoding support |
| **CSV Config** (`csv_config.h`) | Configuration management with encoding options |
| **CSV Utils** (`csv_utils.h`) | Utility functions |

### Arena Management

```c
// Initialize arena with specified size
Arena arena;
ArenaResult result = arena_create(&arena, size_t size);

// Allocate memory from arena
void* ptr;
ArenaResult result = arena_alloc(&arena, size_t size, &ptr);

// Duplicate string in arena
ArenaResult result = arena_strdup(&arena, const char* str, char** result);

// Reset arena for reuse
arena_reset(&arena);

// Clean up arena
arena_destroy(&arena);
```

### Enhanced CSV Reading

```c
// Initialize reader with configuration
CSVReader *reader = csv_reader_init_with_config(&arena, config);

// Navigation and positioning
int has_more = csv_reader_has_next(reader);
long position = csv_reader_get_position(reader);
int seek_result = csv_reader_seek(reader, long position);
csv_reader_rewind(reader);

// Header management
int header_count;
char **headers = csv_reader_get_headers(reader, &header_count);

// Configuration updates
csv_reader_set_config(reader, &arena, new_config);

// Read records
CSVRecord *record = csv_reader_next_record(reader);
```

### Advanced CSV Writing

```c
// Initialize with encoding and BOM support
CSVWriter *writer;
CSVWriterResult result = csv_writer_init(&writer, config, headers, count, &arena);

// Write records with automatic formatting
csv_writer_write_record(writer, fields, field_count);

// Write with field mapping
csv_writer_write_record_map(writer, field_names, field_values, count);

// Utility functions
bool needs_quoting = field_needs_quoting(field, delimiter, enclosure, strict_mode);
bool is_numeric = is_numeric_field(field);
```

## ⚙️ Configuration

### Basic Configuration

```c
CSVConfig *config = csv_config_create(&arena);

// Customize delimiters and quotes
csv_config_set_delimiter(config, ';');        // Default: ','
csv_config_set_enclosure(config, '\'');       // Default: '"'
csv_config_set_escape(config, '\\');          // Default: '"'

// Configure parsing behavior
csv_config_set_trim_fields(config, true);     // Default: false
csv_config_set_skip_empty_lines(config, true); // Default: false
csv_config_set_strict_mode(config, true);     // Default: false
csv_config_set_preserve_quotes(config, false); // Default: false
```

### Advanced Configuration

```c
// Encoding and BOM support
csv_config_set_encoding(config, CSV_ENCODING_UTF8);
csv_config_set_write_bom(config, true);

// File handling
csv_config_set_path(config, "data.csv");
csv_config_set_has_header(config, true);
csv_config_set_offset(config, 100);  // Skip first 100 lines
csv_config_set_limit(config, 1000);  // Process only 1000 records
```

## 🌐 Encoding Support

### Supported Encodings

| Encoding | Constant | BOM Support | Notes |
|----------|----------|-------------|-------|
| UTF-8 | `CSV_ENCODING_UTF8` | ✅ | Unicode, default |
| UTF-16 LE | `CSV_ENCODING_UTF16LE` | ✅ | Unicode |
| UTF-16 BE | `CSV_ENCODING_UTF16BE` | ✅ | Unicode |
| UTF-32 LE | `CSV_ENCODING_UTF32LE` | ✅ | Unicode |
| UTF-32 BE | `CSV_ENCODING_UTF32BE` | ✅ | Unicode |
| ASCII | `CSV_ENCODING_ASCII` | ❌ | Single-byte, no BOM, no Unicode |
| Latin1 | `CSV_ENCODING_LATIN1` | ❌ | Single-byte, no BOM, Western European |

- **ASCII** and **Latin1** are fully supported for both reading and writing. No BOM is written for these encodings. They are suitable for legacy systems and Western European text, but do not support Unicode characters outside their range.

### BOM (Byte Order Mark) Writing

```c
// Enable BOM for UTF encodings
csv_config_set_encoding(config, CSV_ENCODING_UTF8);
csv_config_set_write_bom(config, true);

// BOM bytes are automatically written:
// UTF-8: EF BB BF
// UTF-16LE: FF FE
// UTF-16BE: FE FF
// UTF-32LE: FF FE 00 00
// UTF-32BE: 00 00 FE FF
```

## 🔧 Advanced Features

### Multi-line Field Support

```c
// Automatic handling of quoted multi-line fields
char *content = "name,description\n"
                "\"Product A\",\"A great product\nwith multiple lines\"\n"
                "\"Product B\",\"Another product\"";

// Parser automatically handles multi-line quoted fields
CSVParseResult result = csv_parse_line_inplace(content, &arena, config, 1);
```

### RFC 4180 Quote Escaping

```c
// Proper quote escaping: "" becomes "
char *input = "\"Say \"\"Hello\"\" World\",normal";
// Results in: Say "Hello" World, normal

// Enhanced quote handling in parser
CSVParseResult result = csv_parse_line_inplace(input, &arena, config, 1);
```

### Strict Mode Processing

```c
// Enable strict mode for enhanced validation
csv_config_set_strict_mode(config, true);

// Strict mode features:
// - Fields with spaces are automatically quoted
// - Enhanced validation of field content
// - Stricter RFC 4180 compliance
```

## 🧪 Testing

The library includes comprehensive test coverage:

| Test Suite | Tests | Coverage |
|------------|-------|----------|
| **Arena Tests** | 13 | Memory allocation, alignment, bounds, safety |
| **Config Tests** | 7 | Configuration management, encoding, flags |
| **Utils Tests** | 11 | String utilities, validation, trimming |
| **Parser Tests** | 7 | Core parsing, quotes, multi-line, edge cases |
| **Writer Tests** | 15 | Record writing, BOM, encoding, formatting |
| **Reader Tests** | 8 | Navigation, headers, seeking, positioning |
| **Total** | **60+** | **All components with edge cases** |

### Running Tests

```bash
# Run all tests
make test

# Run specific test suite
make test-arena
make test-config
make test-utils
make test-parser
make test-writer
make test-reader

# Memory leak detection
make valgrind
make valgrind-all

# Performance testing
make benchmark
make stress-test
```

### Test Results Summary

```
✅ Arena Tests: 13/13 passed
✅ Config Tests: 7/7 passed  
✅ Utils Tests: 11/11 passed
✅ Parser Tests: 7/7 passed
✅ Writer Tests: 15/15 passed
✅ Reader Tests: 8/8 passed
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🎉 Total: 60+ tests passed
```

## ⚡ Performance

### Benchmarks

| Operation | Performance | Memory |
|-----------|-------------|---------|
| Parse 1M records | **7.6M ops/sec** | 90% less malloc |
| Write 1M records | **5.2M ops/sec** | Zero fragmentation |
| Memory allocations | **Arena-based** | Predictable cleanup |
| Multi-line parsing | **Optimized** | Streaming support |

### Performance Features

- **Zero-copy parsing** where possible
- **In-place string modification** to avoid allocations
- **Arena-based memory management** for reduced malloc overhead
- **Optimized field parsing** with minimal string operations
- **Streaming processing** for large files
- **Enhanced quote handling** without performance penalty

### Stress Test Results

```bash
# 50,000 iteration stress test
✅ All iterations completed successfully
✅ Zero memory leaks detected
✅ Consistent performance maintained
```

## 🛡️ Memory Safety

**Validated with Valgrind:**

```
✅ Zero memory leaks
✅ Zero memory errors  
✅ Proper allocation/deallocation balance
✅ No buffer overflows or underflows
✅ No uninitialized memory access
```

**Detailed Test Results:**
- **Arena Tests**: 10 allocs, 10 frees, 8,384 bytes - ✅ Clean
- **Config Tests**: 7 allocs, 7 frees, 25,600 bytes - ✅ Clean
- **Utils Tests**: 1 alloc, 1 free, 1,024 bytes - ✅ Clean
- **Parser Tests**: 14 allocs, 14 frees, 34,328 bytes - ✅ Clean
- **Writer Tests**: 47 allocs, 47 frees, 12,661,592 bytes - ✅ Clean
- **Reader Tests**: 6 allocs, 6 frees, 14,256 bytes - ✅ Clean

## 🔧 Error Handling

The library uses comprehensive error codes for robust error handling:

```c
// Arena errors
typedef enum {
    ARENA_OK = 0,
    ARENA_ERROR_NULL_POINTER,
    ARENA_ERROR_INVALID_SIZE,
    ARENA_ERROR_OUT_OF_MEMORY,
    ARENA_ERROR_ALIGNMENT
} ArenaResult;

// Writer errors
typedef enum {
    CSV_WRITER_OK = 0,
    CSV_WRITER_ERROR_NULL_POINTER,
    CSV_WRITER_ERROR_MEMORY_ALLOCATION,
    CSV_WRITER_ERROR_FILE_OPEN,
    CSV_WRITER_ERROR_FILE_WRITE,
    CSV_WRITER_ERROR_INVALID_FIELD_COUNT,
    CSV_WRITER_ERROR_FIELD_NOT_FOUND,
    CSV_WRITER_ERROR_BUFFER_OVERFLOW,
    CSV_WRITER_ERROR_ENCODING
} CSVWriterResult;

// Parser errors with detailed information
typedef struct {
    bool success;
    const char *error;
    int error_line;
    int error_column;
    FieldArray fields;
} CSVParseResult;
```

## 📚 Examples

### Custom Delimiter Processing

```c
CSVConfig *config = csv_config_create(&arena);
csv_config_set_delimiter(config, ';');  // Use semicolon
csv_config_set_enclosure(config, '\''); // Use single quotes
csv_config_set_strict_mode(config, true); // Enable strict validation
```

### Large File Processing with Navigation

```c
// Efficient streaming for large files
CSVReader *reader = csv_reader_init_with_config(&arena, config);

// Skip to specific position
csv_reader_seek(reader, 1000);

// Process with position tracking
while (csv_reader_has_next(reader)) {
    long position = csv_reader_get_position(reader);
    CSVRecord *record = csv_reader_next_record(reader);
    
    printf("Processing record at position %ld\n", position);
    process_record(record);
    
    // Arena automatically manages memory
}
```

### Multi-Encoding File Processing

```c
// Process files with different encodings, including ASCII and Latin1
CSVEncoding encodings[] = {
    CSV_ENCODING_UTF8,
    CSV_ENCODING_UTF16LE,
    CSV_ENCODING_LATIN1, // Now fully supported
    CSV_ENCODING_ASCII   // Now fully supported
};

for (int i = 0; i < 4; i++) {
    csv_config_set_encoding(config, encodings[i]);
    csv_config_set_write_bom(config, true); // No BOM for ASCII/Latin1
    process_csv_file(config);
}
```

### Integration with Other Languages

The library is designed for easy integration:

- **Python**: Use `ctypes` or `cffi`
- **Node.js**: Use N-API
- **PHP**: Direct C extension integration (optimized API)
- **Go**: Use `cgo`
- **Rust**: Use `bindgen`

## 🏗️ Architecture

```
┌─────────────────┐    ┌─────────────────┐
│   CSV Reader    │    │   CSV Writer    │
│  + Navigation   │    │  + Encoding     │
│  + Headers      │    │  + BOM Support  │
│  + Seeking      │    │  + Strict Mode  │
└─────────┬───────┘    └─────────┬───────┘
          │                      │
          └──────┬─────────────────┘
                 │
         ┌───────▼───────┐
         │  CSV Parser   │
         │ + RFC 4180    │
         │ + Multi-line  │
         │ + Quote Esc   │
         └───────┬───────┘
                 │
         ┌───────▼───────┐
         │  CSV Config   │
         │ + Encoding    │
         │ + BOM Flags   │
         │ + Validation  │
         └───────┬───────┘
                 │
    ┌────────────▼────────────┐
    │     Arena Allocator     │
    │   + Memory Safety       │
    │   + Zero Leaks          │
    │   + Performance         │
    └─────────────────────────┘
```

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
git clone https://github.com/csvtoolkit/FastCSV-C.git
cd FastCSV-C
make test
make valgrind
```

### Code Style

- Follow C99 standard
- Use consistent indentation (4 spaces)
- Add tests for new features
- Ensure Valgrind clean runs
- Update documentation for API changes

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📦 Releases

See [Releases](https://github.com/csvtoolkit/FastCSV-C/releases) for downloadable packages and release notes.

### Latest Release Features
- **Production-ready CSV library** with enterprise features
- **Multi-encoding support** with BOM writing
- **Enhanced RFC 4180 compliance** with proper quote escaping
- **Advanced navigation APIs** for CSV readers
- **Memory-safe** with comprehensive Valgrind validation
- **High-performance** with 7.6M+ operations/second
- **Cross-platform support** (Linux, macOS)
- **Complete test suite** with 60+ tests

## 🙏 Acknowledgments

- Built with performance and safety in mind
- Inspired by modern C library design principles
- RFC 4180 compliant implementation
- Tested extensively for production use
- Optimized for integration with multiple programming languages

---

**Made with ❤️ for the C community** 