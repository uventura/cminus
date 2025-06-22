#==========================
#|	   VARIABLES		|
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

# CFLAGS += -lfl  <-- REMOVE -lfl from CFLAGS

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

# Make sure these are properly defined to include all necessary object files for the final link
ALL_OBJS = $(OBJS) $(LEXER_OBJ) $(SYNTACTIC_OBJ)

SRCS = $(shell find $(SRC_DIR) -name '*.c')
SRCS := $(filter-out $(SYNTACTIC_OUT), $(SRCS))
SRCS := $(filter-out $(LEXER_OUT), $(SRCS))

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

#==========================
#		   MAIN		  |
#==========================
all: $(TARGET)

#==========================
#		BUILDING		 |
#==========================
$(TARGET): $(ALL_OBJS) # <-- Use ALL_OBJS here
	@echo "> Building C-Minus project"
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS) -O3 -Icminus -lfl # <-- Add -lfl at the end
	@echo "> Successfully Compiled"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SYNTACTIC_HDR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $< -Icminus

$(LEXER_OBJ): $(LEXER_OUT) $(SYNTACTIC_HDR)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -o $@ -c $< -Icminus

#====================
#|	  LEXER	   |
#====================
gen_lexer:
	@echo "> Generating Lexer File..."
	@flex -o $(LEXER_OUT) $(LEXER_IN)
	@echo "> Successfully generated!"

# This target seems to build a 'lexer' binary. If you just want to generate, 'gen_lexer' is enough.
# If you want to build a standalone lexer executable for testing, this is fine, but it won't be your main 'cminus' target.
lexer: $(LEXER_OBJ) # <-- Only depend on lexer.o and whatever it needs to link
	@echo "Building Lexer..."
	@flex -o $(LEXER_OUT) $(LEXER_IN) # Regenerate, but only if needed
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $(LEXER_OBJ) $(CFLAGS) -O3 -Icminus -DDEBUG -DLEXER_MODE -lfl # <-- Link only lexer.o and -lfl
	@echo "> Successfully generated!"

#====================
#|	  SYNTATIC	|
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

# This target seems to build a 'syntactic' binary. Similar to 'lexer' target, this is for isolated testing.
syntactic: $(LEXER_OBJ) $(SYNTACTIC_OBJ) # <-- Only depend on lexer.o and syntactic.o and what they need to link
	@echo "Building Syntactic..."
	@flex -o $(LEXER_OUT) $(LEXER_IN) # Regenerate
	@bison -Wcounterexamples -d --defines=$(SYNTACTIC_HDR) -o $(SYNTACTIC_OUT) $(SYNTACTIC_IN) # Regenerate
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $(LEXER_OBJ) $(SYNTACTIC_OBJ) $(CFLAGS) -O3 -Icminus -DDEBUG -lfl # <-- Link with -lfl
	@echo "> Successfully generated!"

#====================
#	   SEMANTIC	 |
#====================

SRCS = $(shell find $(SRC_DIR) -name '*.c')
SRCS := $(filter-out $(SYNTACTIC_OUT), $(SRCS))
SRCS := $(filter-out $(LEXER_OUT), $(SRCS))
SRCS += cminus/semantic/semantic.c  

#==========================
#	  CLEAN PROJECT	  |
#==========================
clean:
	@rm -rf $(OBJS) $(LEXER_OBJ) $(BIN_DIR) $(LEXER_OUT) $(SYNTACTIC_OUT) $(SYNTACTIC_HDR) $(SYNTACTIC_OBJ)
	@echo "Cleaned."