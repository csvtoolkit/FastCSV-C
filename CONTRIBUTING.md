# Contributing to FastCSV-C

We appreciate your interest in contributing to FastCSV-C! This document provides guidelines and instructions for contributing to the core C library of the FastCSV project.

## Code Contribution Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## Development Setup

1. Clone your fork:
   ```bash
   git clone https://github.com/csvtoolkit/FastCSV-C.git
   cd FastCSV-C
   ```

2. Set up build environment:
   ```bash
   # Create build directory
   mkdir build && cd build
   
   # Configure with CMake
   cmake ..
   
   # Build
   make
   ```

3. Run tests:
   ```bash
   make test
   ```

## Coding Standards

### General Guidelines

- Use C89/ANSI C for maximum compatibility
- Follow existing code style
- Keep functions focused and under 50 lines where possible
- Use meaningful variable and function names
- Add comments for complex logic
- Include documentation for public API functions

### Naming Conventions

- Functions: lowercase with underscores (`csv_reader_init`)
- Types: CamelCase (`CSVReader`)
- Constants: uppercase with underscores (`CSV_MAX_BUFFER_SIZE`)
- Private functions: prefix with underscore (`_csv_internal_function`)

### Code Style

```c
// Function definition style
int csv_reader_function(CSVReader* reader, const char* arg) {
    // 4-space indentation
    if (reader == NULL) {
        return 0;
    }
    
    // Variables declared at start of blocks
    int result;
    char* buffer;
    
    // Clear error handling
    if (some_condition) {
        return 0;
    }
    
    return 1;
}
```

### Memory Management

- Always free allocated memory
- Check for NULL after allocations
- Use appropriate buffer sizes
- Document memory ownership in function comments
- Handle cleanup in error cases

### Error Handling

- Return 0/NULL for errors, 1/valid pointer for success
- Set appropriate error messages
- Clean up resources on error paths
- Document error conditions in function headers

## Testing

### Unit Tests

- Add tests for new functionality
- Test both success and failure cases
- Test edge cases
- Test memory management
- Follow existing test patterns

### Test File Structure

```c
void test_csv_reader_init(void) {
    // Setup
    CSVConfig* config = csv_config_new();
    
    // Test case
    CSVReader* reader = csv_reader_init_with_config(config);
    assert(reader != NULL);
    
    // Cleanup
    csv_reader_free(reader);
    csv_config_free(config);
}
```

## Documentation

### Function Documentation

```c
/**
 * Initializes a new CSV reader with the given configuration.
 *
 * @param config Pointer to a valid CSVConfig object
 * @return CSVReader* on success, NULL on failure
 * @note Caller is responsible for freeing the returned reader
 */
CSVReader* csv_reader_init_with_config(CSVConfig* config);
```

### README Updates

- Update README.md for new features
- Keep examples current
- Document breaking changes
- Update version numbers

## Pull Request Guidelines

1. Update tests for new functionality
2. Update documentation
3. Follow coding standards
4. Include clear commit messages
5. One feature/fix per PR
6. Reference any related issues

## License

By contributing, you agree that your contributions will be licensed under the MIT License. 