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
