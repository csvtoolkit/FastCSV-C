# LibFastCSV

A high-performance, standalone C library for CSV file handling, providing efficient reading and writing capabilities with minimal memory footprint. While it powers the PHP FastCSV extension, this library is designed to be used in any C project requiring fast and reliable CSV processing.

## Key Benefits

- **Language Agnostic**: Pure C implementation, can be used in any project or language with C bindings
- **Zero Dependencies**: Only requires standard C library
- **Minimal Memory Usage**: Streaming processing with configurable buffer sizes
- **High Performance**: Optimized for speed and efficiency
- **Cross-Platform**: Works on Linux, macOS, Windows, and other POSIX systems

## Use Cases

- Data Processing Applications
- ETL Tools
- Database Import/Export Tools
- Log Processing Systems
- Scientific Computing
- Language Bindings (Python, Ruby, PHP, etc.)
- Embedded Systems
- Command Line Tools

## Features

- Streaming CSV reading and writing
- Minimal memory footprint
- Custom delimiters and enclosures support
- Header row handling
- UTF-8 support
- Error handling with detailed messages
- No external dependencies
- Thread-safe design

## API Overview

### CSV Configuration

```c
// Create and configure CSV settings
CSVConfig* config = csv_config_new();
csv_config_set_filename(config, "data.csv");
csv_config_set_delimiter(config, ',');
csv_config_set_enclosure(config, '"');
csv_config_set_escape(config, '\\');
csv_config_set_has_header(config, true);
```

### Reading CSV Files

```c
// Initialize reader with config
CSVReader* reader = csv_reader_init_with_config(config);

// Get headers
int header_count;
char** headers = csv_reader_get_headers(reader, &header_count);

// Read records
CSVRecord* record;
while ((record = csv_reader_next_record(reader)) != NULL) {
    // Access fields via record->fields[index]
    // Number of fields available in record->field_count
    
    // Process record...
}

// Clean up
csv_reader_free(reader);
csv_config_free(config);
```

### Writing CSV Files

```c
// Initialize writer with config
CSVWriter* writer = csv_writer_init_with_config(config);

// Set headers
const char* headers[] = {"id", "name", "email"};
csv_writer_set_headers(writer, headers, 3);

// Write records
const char* record[] = {"1", "John Doe", "john@example.com"};
csv_writer_write_record(writer, record, 3);

// Clean up
csv_writer_free(writer);
csv_config_free(config);
```

## Building

### Requirements

- C compiler (GCC 4.x+, Clang, MSVC)
- CMake 3.10+ (for building tests)
- Standard C library

### As a Static Library

```bash
gcc -c csv_config.c csv_reader.c csv_writer.c
ar rcs libfastcsv.a csv_config.o csv_reader.o csv_writer.o
```

### As a Shared Library

```bash
gcc -shared -fPIC -o libfastcsv.so csv_config.c csv_reader.c csv_writer.c
```

### Using CMake

```bash
mkdir build && cd build
cmake ..
make
make install
```

## Integration

### Include in Your Project

1. Copy the library files to your project:
   ```
   lib/
   ├── csv_config.c
   ├── csv_config.h
   ├── csv_reader.c
   ├── csv_reader.h
   ├── csv_writer.c
   └── csv_writer.h
   ```

2. Include the headers in your code:
   ```c
   #include "csv_config.h"
   #include "csv_reader.h"
   #include "csv_writer.h"
   ```

### Using with CMake Projects

```cmake
find_package(fastcsv REQUIRED)
target_link_libraries(your_project PRIVATE fastcsv)
```

## Error Handling

The library uses return values to indicate success/failure:
- Functions returning pointers return NULL on failure
- Functions returning int return 0 on failure, 1 on success
- Detailed error messages can be retrieved through config/reader/writer specific error functions

## Thread Safety

The library is thread-safe when each thread uses its own CSVConfig, CSVReader, and CSVWriter instances. Do not share these objects across threads without proper synchronization.

## Performance Tips

1. Use appropriate buffer sizes for your use case (configurable via CSVConfig)
2. Reuse CSVConfig objects when processing multiple files with same settings
3. Close readers and writers as soon as you're done with them
4. For large files, process records in batches
5. Consider memory-mapped files for very large datasets

## Language Bindings

The library is designed to be easily wrapped for other programming languages. Examples of potential bindings:

- Python using ctypes or CFFI
- Ruby using FFI
- Node.js using N-API
- Go using cgo
- Rust using bindgen
- PHP as an extension

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

Developed and maintained by the CSVToolkit Organization. 