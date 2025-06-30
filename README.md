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
make clean && make semantic  && ./bin/semantic < examples/semantic_correct.cm   # expected to pass
make clean && make semantic  && ./bin/semantic < examples/semantic_error.cm   # expects semantic analyzer to detect errors
```

### Semântico
- Informa Erros: 
1. Declaração de variáveis de identificador repetido, quando há outra declaração de mesmo identificador dentro do mesmo escopo 
2. Uso de variável não-declarada
- Informa Warning:
1. Declaração de variável não-usada 