# utf-8-c
This is a library for basic UTF-8 handling in C. Check the [header](utf-8.h)
for the related functions.

## Compilation
```sh
git clone https://github.com/George-Markas/utf-8-c.git
cd utf-8-c
make # or 'make debug' to include debug symbols

# Compiling main.c
cd ..
cc main.c -I./utf-8-c -L./utf-8-c/build -lutf8 -o main
```

## Example
```c
#include <stdio.h>
#include <stdlib.h>
#include "utf-8.h"

void utf8_write(utf8_char encoding, FILE *stream);

int main(void) {
    const char* str = "Space 🚀🪐";

    FILE *fd = fopen("./example.txt", "w+");
    if (!fd) exit(1);

    utf8_char ch;
    uint8_t length;
    while ((length = utf8_next(str, &ch))) {
        utf8_write(ch, fd);
        str += length;
    }

    fclose(fd);

    return 0;
}

// Example function to write UTF-8 encodings to a stream
void utf8_write(const utf8_char encoding, FILE *stream) {
    const uint8_t length = UTF8_CHAR_LENGTH(encoding);
    unsigned char bytes[4] = {0};

    for (uint8_t i = 0; i < length; i++) {
        bytes[i] = (encoding >> (8 * (length - 1 - i))) & 0xFF;
    }

    fwrite(bytes, 1, length, stream);
}
```
