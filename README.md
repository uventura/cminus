# C-Minus

## Lexer

### How to build on Linux?
```shell
make lexer
```

### How to run?
```shell
bin/lexer file.cm
```

### Install Prerequisites
- bison
- make
- gcc
- flex
```
sudo apt-get install bison flex
```

### Test
- semantic
```
./bin/cminus tests/semantic_test.cm  # expected to pass
./bin/cminus tests/semantic_error.cm  # expects semantic analyzer to detect errors
```