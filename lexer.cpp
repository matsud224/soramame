#include "lexer.h"
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "utility.h"
#include <iostream>

using namespace std;

bool iscrlf(char c){
    return (c=='\n' || c=='\r');
}

bool isoperatorchar(char c){
    switch(c){
        case '%':
        case '=':
        case '~':
        case '|':
        case '^':
        case '-':
        case '+':
        case '*':
        case '/':
        case '<':
        case '>':
        case '&':
        case '!':
            return true;
        default:
            return false;
    }
}

void Lexer::GetNextToken()
{
    redo:
    is_met_semicolon=false;
    while(iswblank(lastchar) || iscrlf(lastchar) || lastchar==';'){
        if(lastchar==';'){
            is_met_semicolon=true;
        }
        lastchar=getnextchar();
    }

    //文字列の解析
    if(isalpha(lastchar)){
        CurrentIdentifier=lastchar;
        while(isalnum(lastchar=getnextchar())){
            CurrentIdentifier+=lastchar;
        }

        if(CurrentIdentifier=="func"){
            CurrentToken=token_func;
            return;
        }

        if(CurrentIdentifier=="return"){
            CurrentToken=token_return;
            return;
        }

        if(CurrentIdentifier=="true"){
            CurrentBoolVal=true;
            CurrentToken=token_boolval;
            return;
        }
        if(CurrentIdentifier=="false"){
            CurrentBoolVal=false;
            CurrentToken=token_boolval;
            return;
        }

        CurrentToken=token_identifier;
        return;
    }

    //文字列値の解析
    if(lastchar=='"'){
        CurrentStringVal="";
        while((lastchar=getnextchar())!='"'){
            CurrentStringVal+=lastchar;
        }
        lastchar=getnextchar();
        CurrentToken=token_stringval;
        return;
    }


    //数値の解析（雑）
    if(isdigit(lastchar)){
        bool is_double=false;//小数かどうか
        string numstr;
        do{
            numstr+=lastchar;
            lastchar=getnextchar();
            if(lastchar=='.'){
                is_double=true;
            }
        }while(isdigit(lastchar) || lastchar=='.');

        if(is_double){
            CurrentFloatVal=atof(numstr.c_str());
            CurrentToken=token_floatval;
            return;
        }else{
            CurrentIntVal=atoi(numstr.c_str());
            CurrentToken= token_intval;
            return;
        }
    }

    //演算子（記号３文字連続まで）
    if(isoperatorchar(lastchar)){
        string opstr;
        opstr+=lastchar;
        for(int i=0;i<3;i++){
            lastchar=getnextchar();
            if(i==2){
                break;
            }
            if(!isoperatorchar(lastchar)){
                break;
            }
            opstr+=lastchar;
        }

        CurrentOperator=opstr;

        if(CurrentOperator=="-" && (CurrentToken==token_operator || CurrentToken=='(' || CurrentToken=='{')){
            //一つ前のトークン（CurrentToken:まだ前回のが残っている）が演算子、開きカッコの場合（？うまくいかないかも）単項演算子のマイナスとする
            if(isdigit(lastchar)){
                bool is_double=false;//小数かどうか
                string numstr;
                do{
                    numstr+=lastchar;
                    lastchar=getnextchar();
                    if(lastchar=='.'){
                        is_double=true;
                    }
                }while(isdigit(lastchar) || lastchar=='.');

                if(is_double){
                    CurrentFloatVal=(-1)*atof(numstr.c_str());
                    CurrentToken=token_floatval;
                    return;
                }else{
                    CurrentIntVal=(-1)*atoi(numstr.c_str());
                    CurrentToken= token_intval;
                    return;
                }
            }
        }

        CurrentToken=token_operator;
        return;
    }

    //コメント「読み飛ばし
    if(lastchar=='#'){
        do{
            lastchar=getnextchar();
        }while(lastchar!=EOF && !iscrlf(lastchar));

        if(lastchar!=EOF){
            goto redo;
        }
    }

    if(lastchar==EOF){
        CurrentToken=token_eof;
        return;
    }

    CurrentToken=lastchar;
    lastchar=getnextchar();
    return;
}

bool Lexer::isClosureAfterParen(){
    int open=0;
    unsigned int i;
    for(i=nowpos+1;i<rawcode.size();i++){
        if(rawcode[i]=='('){
            open++;
        }else if(rawcode[i]==')'){
            if(open==0){
                break;
            }else{
                open--;
            }
        }
    }
    i++;
    if(i>=rawcode.size()){
        return false;
    }
    for(unsigned int j=i;j<rawcode.size();j++){
        if(!(iswblank(rawcode[j]) || iscrlf(rawcode[j]) || rawcode[j]==';')){
            if(rawcode[j]=='='){
                if(j+1!=rawcode.size() && rawcode[j+1]=='>'){
                    return true;
                }
            }
        }
    }

    return false;
}

char Lexer::getnextchar()
{
    if(nowpos==rawcode.size()){
        return EOF;
    }
    return rawcode[nowpos++];
}
