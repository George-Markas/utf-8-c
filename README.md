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
    const utf8_str str1 = utf8_str_make("🚀 to the ");
    const utf8_str str2 = utf8_str_make("🌙 Moon");
    const utf8_str str3 = utf8_str_join(str1, str2);

    FILE *fd = fopen("./example.txt", "w+");
    if (!fd) exit(1);

    utf8_char ch;
    uint8_t length;
    size_t offset = 0;
    while ((length = utf8_next(str3.str + offset, &ch))) {
        utf8_write(ch, fd);
        offset += length;
    }

    fclose(fd);
    utf8_str_free(str3);

    return 0;
}

// Example function to write UTF-8 encodings to a stream
void utf8_write(const utf8_char encoding, FILE *stream) {
    const uint8_t length = utf8_char_length(encoding);
    unsigned char bytes[4] = {0};

    for (uint8_t i = 0; i < length; i++) {
        bytes[i] = (encoding >> (8 * (length - 1 - i))) & 0xFF;
    }

    fwrite(bytes, 1, length, stream);
}
```
