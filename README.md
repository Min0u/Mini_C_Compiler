# Mini C Compiler #

## Introduction ##

This is a Mini C Compiler project. It translates a simplified subset of the C language (STRUCIT-frontend) into an intermediate language resembling assembly (STRUCIT-backend).  
The compiler generates three-address code from the source code written in STRUCIT-frontend.

## Directory Structure ##

`Resources/` - Contains information about the project.  
`Resources/Example/` - Contains example of STRUCIT-frontend code and its corresponding STRUCIT-backend code.    

`src/` - Contains the source code of the project.   
`src/Lexical_Analysis/` - Contains the lexical analysis code (Lex).
`src/Semantic_Analysis/` - Contains the semantic analysis code.    
`src/Syntax_Analysis/` - Contains the syntax analysis code(Yacc).

`Tests/` - Contains test cases for the project.     
`Tests/Output/` - Contains the output of the test cases.

## Error Handling ##

The Mini C Compiler has built-in error handling for both syntax and semantic errors. 

For syntax errors, the Bison parser will automatically detect any violations of the grammar rules. If an error is detected, the parser will attempt to recover and continue parsing, if possible. Custom error messages can be provided by defining the `yyerror` function in the Bison grammar file.

For semantic errors, such as type mismatches or undeclared variables, the compiler uses a symbol table to keep track of variable and type declarations. If a semantic error is detected, an error message will be printed to the console, and the compilation process will be halted.

Please note that while the compiler is designed to handle a wide range of errors, it may not catch all possible errors in your STRUCIT-frontend code. Some complex or obscure errors may go undetected.

To see the error handling in action, you can intentionally introduce errors into your STRUCIT-frontend code and observe the error messages produced by the compiler.

## How to Run ##

Make sure you have `flex`, `bison`, and `gcc` installed on your system.
If not, you can install them using the following commands        
```
sudo apt-get install flex                       
sudo apt-get install bison
sudo apt-get install gcc
```

1. Clone the repository.
2. Navigate to the `Mini_C_Compiler` directory.
3. Run the following command to get all available commands: `make help`

## If makefile is not working ##
Use the following command to generate the executables :

Create the following directories :        
```
mkdir -p src/build/BE
mkdir -p src/build/FE
mkdir -p Tests/Output
```

STRUCIT-backend :             
```
bison -Wcounterexamples -o src/build/BE/y.tab.c -d src/Syntax_Analysis/structbe.y
flex -o src/build/BE/lex.yy.c src/Lexical_Analysis/ANSI-BE.l
gcc -o src/build/BE/strucit_backend src/build/BE/y.tab.c src/build/BE/lex.yy.c -g -lfl -W -Wall -Wextra -Wpedantic
```

STRUCIT-frontend :    
```
bison -Wcounterexamples -o src/build/FE/y.tab.c -d src/Syntax_Analysis/structfe.y
flex -o src/build/FE/lex.yy.c src/Lexical_Analysis/ANSI-C.l
gcc -o src/build/FE/strucit_frontend src/Semantic_Analysis/ast.c src/Semantic_Analysis/code.c src/Semantic_Analysis/hashmap.c src/Semantic_Analysis/stack.c src/Semantic_Analysis/symbol.c src/build/FE/y.tab.c src/build/FE/lex.yy.c -g -lfl -W -Wall -Wextra -Wpedantic
```

To compile the STRUCIT-frontend file, use the following command : 
`
./src/build/FE/strucit_frontend <path_to_input_file> <path_to_output_file>
`

