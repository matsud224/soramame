#ƒRƒ}ƒ“ƒh
all :
	g++ -c ast.cpp
	g++ -c lexer.cpp
	g++ -c main.cpp
	g++ -c utility.cpp
	g++ -c vm.cpp
	flex lexer.l
	bison -v -d -y parser.y -o parser.cc
	gcc -c lex.yy.c -o lex.yy.o
	g++ ast.o lexer.o main.o utility.o vm.o lex.yy.o parser.cc -o language1.exe

cleanall :
	-del /Q *.o
	-del /Q parser.cc
