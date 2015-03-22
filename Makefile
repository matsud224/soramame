OBJS = main.o lexer.o ast.o utility.o vm.o lex.yy.o parser.cc

#�t�@�C�������K��
lex.yy.o : lex.yy.c
	gcc -c lex.yy.c -o lex.yy.o
	
lex.yy.c : lexer.l
	flex lexer.l
	
parser.cc : parser.y
	bison -d -v -y parser.y -o parser.cc
	
	
#��ʋK��
%.o : %.cpp
	g++ -g -Wall $*.cpp -o $*.o

#�R�}���h
all : language1.exe

language1.exe : ${OBJS}
	g++ ${OBJS} -o language1.exe

clean : 
	del /Q *.o
	del /Q parser.cc
