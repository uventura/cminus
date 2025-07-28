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
CFLAGS += -D_GNU_SOURCE  # For POSIX functions
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

SEMANTIC_BIN = semantic
SEMANTIC_MODE_FLAG = SEMANTIC_MODE

CODEGEN_BIN = codegen
CODEGEN_MODE_FLAG = CODEGEN_MODE

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
$(TARGET): $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_OBJ)
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
#|      LEXER       |
#====================
gen_lexer:
	@echo "> Generating Lexer File..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@echo "> Successfully generated!"

lexer: $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_HDR)
	@echo "Building Lexer..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus -DDEBUG -D$(LEXER_MODE_FLAG)
	@echo "> Successfully generated!"

#====================
#|    SYNACTIC    |
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

#====================
#|     SEMANTIC     |
#====================
semantic: $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_OBJ) $(SYNTACTIC_HDR)
	@echo "Building Semantic..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@bison -Wcounterexamples -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$(SEMANTIC_BIN) $^ $(CFLAGS) -O3 -Icminus -DDEBUG -D$(SEMANTIC_MODE_FLAG)
	@echo "> Successfully generated!"

#====================
#|     CODEGEN      |
#====================
codegen: $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_OBJ) $(SYNTACTIC_HDR)
	@echo "Building Codgen..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@bison -Wcounterexamples -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$(CODEGEN_BIN) $^ $(CFLAGS) -O3 -Icminus -DDEBUG -D$(CODEGEN_MODE_FLAG)
	@echo "> Successfully generated!"

#==========================
#      CLEAN PROJECT      |
#==========================
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(LEXER_OUT) $(SYNTACTIC_OUT) $(SYNTACTIC_HDR)
	@echo "Cleaned."