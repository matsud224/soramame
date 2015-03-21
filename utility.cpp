#include <cstdlib>
#include <iostream>
#include <string>
#include "utility.h"
#include "lexer.h"


//�w�肵���s�ԍ��̃e�L�X�g��Ԃ��܂�
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
        cout<<"���ӏ�:"<<endl<<GetLine(lex->rawcode,lex->CurrentLineNumber)<<endl;
        //�s���̃G���[�ӏ��ɁO�}�[�N������
        for(int i=0;i<lex->CurrentCharPosition-1/*lexer�͈ꑫ�ɏo�Ă�̂�*/;i++){
            cout<<" ";
        }
        cout<<"^"<<endl;
    }
    abort();
    return;
}
