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
    token_return=-9
};

class Lexer{
public:
    string CurrentIdentifier;
    string CurrentOperator;
    int CurrentIntVal;
    bool CurrentBoolVal;
    float CurrentFloatVal;
    string CurrentStringVal;

    int CurrentToken;

    bool is_met_semicolon; //GetNextToken()�����s�������ɃZ�~�R������ǂݔ�΂������H�@�@return;�̌��o�p�B
    bool isClosureAfterParen(); //����������T���āA�������̌��=>�����邩�𒲂ׂ�i�N���[�W���̌��o�p�j

    void GetNextToken();

    void SetOrigin(){nowpos=0;lastchar=' ';CurrentToken=0;}

    Lexer(string &code):rawcode(code){lastchar=' ';nowpos=0;CurrentToken=0;};

private:
    string &rawcode;
    char lastchar;
    unsigned int nowpos;

    char getnextchar();
};
