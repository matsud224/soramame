#include <cstdlib>
#include <iostream>
#include <string>
#include "utility.h"
#include "lexer.h"


//指定した行番号のテキストを返します
string GetLine(std::string target,int n){
    int index=0;
    string result="";
    int linecount=1;
    while(target[index]!=NULL){
        if(iseol(target[index])){
            linecount++;
            index++;
            continue;
        }
        if(linecount==n){
            result+=target[index];
        }else if(linecount>n){
            break;
        }
        index++;
    }
    return result;
}

void error(Lexer *lex,std::string msg)
{
    cout<<msg<<endl;
    if(lex){
        cout<<"問題箇所:"<<endl<<GetLine(lex->rawcode,lex->CurrentLineNumber)<<endl;
        //行内のエラー箇所に＾マークを入れる
        for(int i=0;i<lex->CurrentCharPosition-1/*lexerは一足に出てるので*/;i++){
            cout<<" ";
        }
        cout<<"^"<<endl;
    }
    abort();
    return;
}
