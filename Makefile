#==========================
#|       VARIABLES        |
#==========================
TARGET=cminus
BIN_DIR=bin

CC=gcc

CFLAGS=-W
CFLAGS+=-Wall
CFLAGS+=-ansi
CFLAGS+=-pedantic
CFLAGS+=-std=c99
CFLAGS+=-g
CFLAGS+=-Wno-unused-parameter
CFLAGS+=-Wno-implicit-fallthrough
CFLAGS+=-lfl

OBJ_DIR=obj

LEXER_BIN = lexer
LEXER_IN = cminus/lexer/lexer.l
LEXER_OUT = cminus/lexer/lexer.c
LEXER_MODE_FLAG = LEXER_MODE

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

#==========================
#           MAIN          |
#==========================
all: $(TARGET)

#==========================
#        BUILDING         |
#==========================
$(TARGET): $(OBJS)
	@echo "> Building C-Minus project"
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus
	@echo "> Successfully Compiled"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $< -Isrc

#====================
#|		LEXER		|
#====================
gen_lexer:
	@echo "> Generating Lexer File..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@echo "> Successfully generated!"

lexer: $(OBJS)
	@echo "Building Lexer..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus -DDEBUG -DLEXER_MODE
	@echo "> Successfully generated!"

#==========================
#      CLEAN PROJECT      |
#==========================
clean:
	@rm -rf $(OBJS) $(BIN_DIR)
