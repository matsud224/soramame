#include <iostream>
#include "lexer.h"
#include <string>
#include <vector>
#include "ast.h"
#include "vm.h"
#include "utility.h"
#include <conio.h>

using namespace std;

int yyparse();

int main(){
    yyparse();
}

/*
int main(int argc, char* argv[])
{
    if(argc==2){
        string code(argv[1]);
        cout<<"����:"<<endl<<code<<endl<<endl;
        Lexer lexer(code);


        lexer.SetOrigin();
        cout<<"AST�𐶐����Ă��܂�..."<<endl;;
        Parser parser(&lexer);
        parser.Parse();
        cout<<"����"<<endl;
        cout<<"�^���`�F�b�N���Ă��܂�...";
        parser.TypeCheck();
        cout<<"����"<<endl;
        cout<<"�R�[�h�𐶐����Ă��܂�...";
        parser.Codegen();
        cout<<"����"<<endl<<endl<<"���s���܂�..."<<endl;
        CodegenInfo cgi=parser.GetCGInfo();
        VM vm(&cgi);
        vm.Run();
        cout<<endl<<"�I�����܂����B"<<endl;
    }else if(argc==3){
        string code(argv[2]);
        if(string(argv[1])=="-t"){
            cout<<"����:"<<endl<<code<<endl<<endl;
            Lexer lexer(code);


            lexer.SetOrigin();
            cout<<"AST�𐶐����Ă��܂�..."<<endl;;
            Parser parser(&lexer);
            parser.Parse();
            cout<<"����"<<endl;
            cout<<"�^���`�F�b�N���Ă��܂�...";
            parser.TypeCheck();
            cout<<"����"<<endl;
            cout<<"�R�[�h�𐶐����Ă��܂�...";
            parser.Codegen();
            cout<<"����"<<endl;
            CodegenInfo cgi=parser.GetCGInfo();
        }else if(string(argv[1])=="-s"){
            cout<<"�������ł�"<<endl;
        }else{
            error(NULL,string(argv[1])+":�����ȃI�v�V�����ł��B");
        }
	}else{
		error(NULL,"�R�}���h���C���������m�F���ĉ������B");
		return -1;
	}

	getch();

    return 0;
}
*/



    /*lexer�̃e�X�g�p
    cout<<"lexer:"<<endl;
    while(lexer.CurrentToken!=token_eof){
        lexer.GetNextToken();
        switch(lexer.CurrentToken){
            case token_eof:
                cout<<"<eof>"<<endl;
                break;
            case token_func:
                cout<<"<func>"<<endl;
                break;
            case token_identifier:
                cout<<"<identifier> "<<lexer.CurrentIdentifier<<endl;
                break;
            case token_intval:
                cout<<"<intval> "<<lexer.CurrentIntVal<<endl;
                break;
            case token_floatval:
                cout<<"<floatval> "<<lexer.CurrentFloatVal<<endl;
                break;
            case token_operator:
                cout<<"<operator> "<<lexer.CurrentOperator<<endl;
                break;
            case token_boolval:
                cout<<"<bool> "<<lexer.CurrentFloatVal<<endl;
                break;
            case token_stringval:
                cout<<"<stringval>"<<lexer.CurrentStringVal<<endl;
                break;
            default:
                cout<<"<char>"<<(char)(lexer.CurrentToken)<<endl;
                break;
        }
    }
    cout<<endl;*/

