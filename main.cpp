#include <iostream>
#include "lexer.h"
#include <string>
#include <vector>
#include "ast.h"
#include "vm.h"

using namespace std;

int main()
{
    string code="func main(){printint(5+6);return}";
    cout<<"code:"<<endl<<code<<endl<<endl;
    Lexer lexer(code);
    /*
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

    lexer.SetOrigin();
    cout<<"ASTを生成しています..."<<endl;
    Parser parser(&lexer);
    parser.Parse();
    cout<<"完了"<<endl;
    cout<<"型をチェックしています..."<<endl;
    parser.TypeCheck();
    cout<<"完了"<<endl;
    cout<<endl<<endl<<"コード生成を開始します..."<<endl;
    parser.Codegen();
    CodegenInfo cgi=parser.GetCGInfo();/*
    VM vm(&cgi);
    cout<<"完了"<<endl<<endl;
    vm.OutputCode();
    vm.Run();
    cout<<"終了しました。"<<endl;*/
    return 0;
}
