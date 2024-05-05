CC = gcc
LEX = flex
BISON = bison
CFLAGS = -g -lfl -W -Wall -Wextra -Wpedantic

# Valgrind : check memory leaks
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
HEAD = | head -n 100

# Back-End
OUTPUT_DIR_BE = build/BE
SOURCE_DIR_BE_YACC = src/Syntax_Analysis/structbe.y
SOURCE_DIR_BE_LEX = src/Lexical_Analysis/ANSI-BE.l

# Front-End
OUTPUT_DIR_FE = build/FE
SOURCE_DIR_FE_YACC = src/Syntax_Analysis/structfe.y
SOURCE_DIR_FE_LEX = src/Lexical_Analysis/ANSI-C.l
ADDITIONAL_FE_FILES = src/Semantic_Analysis/*.c

# Test
TEST_DIR = Tests
TEST_OUTPUT_DIR = $(TEST_DIR)/Output

# Test files
TEST_FILES := $(wildcard $(TEST_DIR)/*.c)

# Test outputs
TEST_OUTPUT_FILES := $(wildcard $(TEST_OUTPUT_DIR)/*.c)

# Default
.DEFAULT_GOAL := all

all: strucit_backend strucit_frontend

# Back-End
strucit_backend: $(OUTPUT_DIR_BE)/lex.yy.c $(OUTPUT_DIR_BE)/y.tab.c
	$(CC) -o $(OUTPUT_DIR_BE)/$@ $(OUTPUT_DIR_BE)/y.tab.c $(OUTPUT_DIR_BE)/lex.yy.c $(CFLAGS)

$(OUTPUT_DIR_BE)/lex.yy.c: $(SOURCE_DIR_BE_LEX) | build_dir_be
	$(LEX) -o $@ $<

$(OUTPUT_DIR_BE)/y.tab.c: $(SOURCE_DIR_BE_YACC) | build_dir_be
	$(BISON) -Wcounterexamples -o $@ -d $<

	@echo "\033[0;32mCompilation of the Back-End done\033[0m"

# Front-End
strucit_frontend: $(OUTPUT_DIR_FE)/lex.yy.c $(OUTPUT_DIR_FE)/y.tab.c $(ADDITIONAL_FE_FILES)
	$(CC) -o $(OUTPUT_DIR_FE)/$@ $(ADDITIONAL_FE_FILES) $(OUTPUT_DIR_FE)/y.tab.c $(OUTPUT_DIR_FE)/lex.yy.c $(CFLAGS)

$(OUTPUT_DIR_FE)/lex.yy.c: $(SOURCE_DIR_FE_LEX) | build_dir_fe
	$(LEX) -o $@ $<

$(OUTPUT_DIR_FE)/y.tab.c: $(SOURCE_DIR_FE_YACC) | build_dir_fe
	$(BISON) -Wcounterexamples -o $@ -d $<

	@echo "\033[0;32mCompilation of the Front-End done\033[0m"

build_dir_be:
	mkdir -p $(OUTPUT_DIR_BE)

build_dir_fe:
	mkdir -p $(OUTPUT_DIR_FE)
	mkdir -p $(TEST_OUTPUT_DIR)

# Test all files in the Test directory
test:
	@for file in $(TEST_FILES); do \
		./$(OUTPUT_DIR_FE)/strucit_frontend $$file $(TEST_OUTPUT_DIR)/$$(basename $$file .c)_backend.c; \
		if [ $$? -eq 0 ]; then \
			echo "\033[0;32m$$file : success\033[0m"; \
		else \
			echo "\033[0;31m$$file : error\033[0m"; \
		fi; \
		echo ""; \
	done

	@echo "\033[0;32mAll tests done\033[0m"

# Test a specific file
test_file:
	@read -p "Enter the file name and the output file name (ex: ./Tests/test.c ./Tests/Output/test.c) : " file output; \
	./$(OUTPUT_DIR_FE)/strucit_frontend $$file $$output

	@if [ $$? -eq 0 ]; then \
		echo "\033[0;32m\success\033[0m"; \
	else \
		echo "\033[0;31merror\033[0m"; \
	fi

	@echo "\033[0;32mTest done\033[0m"

# Test all outputs
test_outputs:
	@for file in $(TEST_OUTPUT_FILES); do \
		./$(OUTPUT_DIR_BE)/strucit_backend < $$file; \
		if [ $$? -eq 0 ]; then \
			echo "\033[0;32m$$file : success\033[0m"; \
		else \
			echo "\033[0;31m$$file : error\033[0m"; \
		fi; \
		echo ""; \
	done

	@echo "\033[0;32mAll outputs done\033[0m"

# Check memory leaks
valgrind_test:
	@for file in $(TEST_FILES); do \
		$(VALGRIND) ./$(OUTPUT_DIR_FE)/strucit_frontend $$file $(TEST_OUTPUT_DIR)/$$(basename $$file .c)_backend.c $(HEAD); \
		if [ $$? -eq 0 ]; then \
			echo "\033[0;32m$$file : success\033[0m"; \
		else \
			echo "\033[0;31m$$file : error\033[0m"; \
		fi; \
		echo ""; \
	done

	@echo "\033[0;32mValgrind test done\033[0m"

# Check memory leaks for a specific file
valgrind_test_file:
	@read -p "Enter the file name and the output file name (ex: ./Tests/test.c ./Tests/Output/test.c) : " file output; \
	$(VALGRIND) ./$(OUTPUT_DIR_FE)/strucit_frontend $$file $$output $(HEAD); \
	if [ $$? -eq 0 ]; then \
		echo "\033[0;32msuccess\033[0m"; \
	else \
		echo "\033[0;31merror\033[0m"; \
	fi

	@echo "\033[0;32mValgrind test done\033[0m"

# Compile and check memory leaks for all files in the Test directory
make_valgrind:
	make && make valgrind_test

# Compile and check memory leaks for a specific file
make_valgrind_file:
	make && make valgrind_test_file

# Clean all build files
clean:
	rm -rf $(OUTPUT_DIR_BE)/*.c $(OUTPUT_DIR_BE)/*.h $(OUTPUT_DIR_BE)/BE
	rm -rf $(OUTPUT_DIR_FE)/*.c $(OUTPUT_DIR_FE)/*.h $(OUTPUT_DIR_FE)/FE
	rm -rf $(TEST_OUTPUT_DIR)/*

	@echo "\033[0;32mAll files have been removed\033[0m"

# Clean all build files + executables
clean_all:
	@if [ -d $(OUTPUT_DIR_BE) ]; then \
		rm -rf $(OUTPUT_DIR_BE); \
	fi
	@if [ -d $(OUTPUT_DIR_FE) ]; then \
		rm -rf $(OUTPUT_DIR_FE); \
	fi
	@if [ -d $(TEST_OUTPUT_DIR) ]; then \
		rm -rf $(TEST_OUTPUT_DIR); \
	fi
	@echo "\033[0;32mAll files and executables have been removed\033[0m"

# Clean all tests files
clean_test:
	rm -rf $(TEST_OUTPUT_DIR)/*

	@echo "\033[0;32mAll test files have been removed\033[0m"

# Help
help:

	@echo "\033[0;35mWelcome to the Mini C Compiler :)\033[0m"
	@echo ""
	@echo "Usage : make \033[1;36m[command]\033[0m"
	@echo "- \033[1;36mall\033[0m : compile the Back-End and the Front-End"
	@echo "- \033[1;36mstrucit_backend\033[0m : compile the Back-End"
	@echo "- \033[1;36mstrucit_frontend\033[0m : compile the Front-End"
	@echo "- \033[1;36mtest\033[0m : test all files in the Test directory"
	@echo "- \033[1;36mtest_file\033[0m : test a specific file"
	@echo "- \033[1;36mtest_outputs\033[0m : test all outputs"
	@echo "- \033[1;36mvalgrind_test\033[0m : check memory leaks for all files in the Test directory"
	@echo "- \033[1;36mvalgrind_test_file\033[0m : check memory leaks for a specific file"
	@echo "- \033[1;36mmake_valgrind\033[0m : compile and check memory leaks for all files in the Test directory"
	@echo "- \033[1;36mmake_valgrind_file\033[0m : compile and check memory leaks for a specific file"
	@echo "- \033[1;36mclean\033[0m : remove all build files"
	@echo "- \033[1;36mclean_all\033[0m : remove all build files and executables"
	@echo "- \033[1;36mclean_test\033[0m : remove all test files"
	@echo "- \033[1;36mhelp\033[0m : display this help"
	@echo ""
	@echo "\033[0;35mEnjoy !\033[0m"