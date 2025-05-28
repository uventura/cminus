#==========================
#|       VARIABLES        |
#==========================
TARGET = cminus
BIN_DIR = bin

CC = gcc

CFLAGS = -W
CFLAGS += -Wall
CFLAGS += -ansi
CFLAGS += -pedantic
CFLAGS += -std=c99
CFLAGS += -g
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -lfl

OBJ_DIR = obj

LEXER_BIN = lexer
LEXER_IN = cminus/lexer/lexer.l
LEXER_OUT = cminus/lexer/lexer.c
LEXER_OBJ = $(OBJ_DIR)/lexer.o
LEXER_MODE_FLAG = LEXER_MODE

SYNTACTIC_BIN = syntactic
SYNTACTIC_IN = cminus/syntactic/syntactic.y
SYNTACTIC_OUT = cminus/syntactic/syntactic.tab.c
SYNTACTIC_HDR = cminus/syntactic/syntactic.tab.h
SYNTACTIC_OBJ = $(OBJ_DIR)/syntactic.o
SYNTACTIC_MODE_FLAG = SYNTACTIC_MODE

SRC_DIR = cminus

SRCS = $(shell find $(SRC_DIR) -name '*.c')
SRCS := $(filter-out $(SYNTACTIC_OUT), $(SRCS))
SRCS := $(filter-out $(LEXER_OUT), $(SRCS))

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SYNTACTIC_HDR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $< -Icminus

$(LEXER_OBJ): $(LEXER_OUT) $(SYNTACTIC_HDR)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -o $@ -c $< -Icminus

#====================
#|		LEXER		|
#====================
gen_lexer:
	@echo "> Generating Lexer File..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@echo "> Successfully generated!"

lexer: $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_HDR)
	@echo "Building Lexer..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus -DDEBUG -DLEXER_MODE
	@echo "> Successfully generated!"

#====================
#|		SYNTATIC	|
#====================
gen_syntactic:
	@echo "> Generating Syntactic Files..."
	@bison -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN)
	@echo "> Successfully generated!"

$(SYNTACTIC_HDR) $(SYNTACTIC_OUT): $(SYNTACTIC_IN)
	@echo "> Generating Syntactic Files..."
	@bison -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN)

$(SYNTACTIC_OBJ): $(SYNTACTIC_OUT)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

syntactic: $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_OBJ) $(SYNTACTIC_HDR)
	@echo "Building Syntactic..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@bison -Wcounterexamples -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus -DDEBUG
	@echo "> Successfully generated!"

#==========================
#      CLEAN PROJECT      |
#==========================
clean:
	@rm -rf $(OBJS) $(LEXER_OBJ) $(BIN_DIR) $(LEXER_OUT) $(SYNTACTIC_OUT) $(SYNTACTIC_HDR) $(SYNTACTIC_OBJ)
	@echo "Cleaned."
