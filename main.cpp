#include <iostream>
#include "lexer.h"
#include <string>
#include <vector>
#include "ast.h"
#include "vm.h"
#include <conio.h>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc==2){
        string code(argv[1]);
        cout<<"入力:"<<endl<<code<<endl<<endl;
        Lexer lexer(code);


        lexer.SetOrigin();
        cout<<"ASTを生成しています..."<<endl;;
        Parser parser(&lexer);
        parser.Parse();
        cout<<"完了"<<endl;
        cout<<"型をチェックしています...";
        parser.TypeCheck();
        cout<<"完了"<<endl;
        cout<<"コードを生成しています...";
        parser.Codegen();
        cout<<"完了"<<endl<<endl<<"実行します..."<<endl;
        CodegenInfo cgi=parser.GetCGInfo();
        VM vm(&cgi);
        vm.Run();
        cout<<endl<<"終了しました。"<<endl;
    }else if(argc==3){
        string code(argv[2]);
        if(string(argv[1])=="-t"){
            cout<<"入力:"<<endl<<code<<endl<<endl;
            Lexer lexer(code);


            lexer.SetOrigin();
            cout<<"ASTを生成しています..."<<endl;;
            Parser parser(&lexer);
            parser.Parse();
            cout<<"完了"<<endl;
            cout<<"型をチェックしています...";
            parser.TypeCheck();
            cout<<"完了"<<endl;
            cout<<"コードを生成しています...";
            parser.Codegen();
            cout<<"完了"<<endl;
            CodegenInfo cgi=parser.GetCGInfo();
        }else if(string(argv[1])=="-s"){
            cout<<"未実装です"<<endl;
        }else{
            error(string(argv[1])+":無効なオプションです。");
        }
	}else{
		error("コマンドライン引数を確認して下さい。");
		return -1;
	}

	getch();

    return 0;
}




    /*lexerのテスト用
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
