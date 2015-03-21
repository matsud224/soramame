#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

enum Token{
    token_eof=-1,
    token_func=-2,
    token_identifier=-3,
    token_intval=-4,
    token_operator=-5, //�L��3����
    token_floatval=-6,
    token_boolval=-7,
    token_stringval=-8,
    token_return=-9,
    token_var=-10,
    token_eol=-11
};

bool iseol(char);

class Lexer{
public:
    int CurrentLineNumber;
    int CurrentCharPosition; // ���݂̍s���ł̍��[����̈ʒu

    string CurrentIdentifier;
    string CurrentOperator;
    int CurrentIntVal;
    bool CurrentBoolVal;
    float CurrentFloatVal;
    string CurrentStringVal;

    int CurrentToken;

    bool isClosureAfterParen(); //����������T���āA�������̌��=>�����邩�𒲂ׂ�i�N���[�W���̌��o�p�j

    void GetNextToken();

    void SetOrigin(){nowpos=0;lastchar=' ';CurrentToken=0;}

    Lexer(string &code):rawcode(code){lastchar=' ';nowpos=0;CurrentToken=0;CurrentLineNumber=1;CurrentCharPosition=-1;};

    string &rawcode;
private:

    char lastchar;
    unsigned int nowpos;

    char getnextchar();
};
