NAME:=cdemon
ENTRY:=$(NAME)

LD_LIBRARY_PATH:=/usr/local/lib
INSTALL_PATH:=/usr/bin
INCLUDE:=/usr/local/include

SRC_DIR:=src
OBJ_DIR:=obj
DEP_DIR:=$(OBJ_DIR)
HEAD_DIR:=include
DOCS_DIR:=docs

INC_DIR:=.

TEST:=test

SRCFILES:=$(shell find $(SRC_DIR) -type f -name "*.c")
OBJFILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCFILES))
DEPFILES := $(patsubst $(SRC_DIR)/%.c,$(DEP_DIR)/%.d,$(SRCFILES))

CC:=gcc
CFLAGS:=-std=gnu18 $(addprefix -I,$(INC_DIR))
INSTALL:=install

all: $(NAME)

.PHONY: all run docs clean

$(NAME): $(OBJFILES)
	@echo "Building Final Binary"
	@$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $@ -lcol -ltpool -lhmap -lqueue -lvector -lpthread
	@echo "Done"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c Makefile $(OBJ_DIR)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
	@echo "Compiling $< file"

$(OBJ_DIR):
	@mkdir $(OBJ_DIR) 2> /dev/null

run: install
	@ulimit -c unlimited
	@$(CC) $(CFLAGS) -o ./examples/$(TEST).o ./examples/$(TEST).c -lcol -ltpool -lhmap -lqueue -lvector -lpthread
	@echo "------------"
	@./examples/$(TEST).o
	@ulimit -c 0

install: $(NAME)
	@echo "Installing..."
	sudo cp $(NAME) $(INSTALL_PATH)
	@echo "Done!"

docs:
	@doxygen 1> /dev/null
	@echo "Documentation created!"
	@echo ""
	@echo "Copy the following link and open in a browser:"
	@echo file://$(shell pwd)/$(DOCS_DIR)/html/index.html

clean:
	@rm -rf $(NAME) $(TEST).o $(OBJ_DIR) $(DEP_DIR) ./examples/$(TEST).o $(DOCS_DIR)

-include $(DEPFILES)