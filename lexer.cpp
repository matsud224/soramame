#include <regex>
#include "lexer.h"
#include "common.h"
#include <iostream>


TokenValue Lexer::dummy;

pair<Symbol,TokenValue> Lexer::Get()
{
    re_get:

    if(rawcode[curr_index]==NULL){
		return pair<Symbol,TokenValue>(INPUTEND,dummy);
    }

	cmatch match;

    //ルールをひとつずつ上から順に試していき，マッチしたら返す
    for(int i=0;i<TOKENRULECOUNT;i++){
        if(TOKENRULE[i].state!=curr_state){
            continue;
        }
        if(regex_match(&(rawcode[curr_index]),match,regex_objs[i])){
            //現在の読み取り開始位置にマッチしたのか？
            if(match.position(i)!=0){
                //離れた位置でマッチした
                continue;
            }

            curr_index+=match.length();

            if(TOKENRULE[i].hasValue==true){
				return TOKENRULE[i].callback(match.str(),this);
            }else{
				if(TOKENRULE[i].callback!=NULL){
					TOKENRULE[i].callback(match.str(),this);
				}
				goto re_get;
            }

        }
    }

    //マッチするものがなかった
    throw NoMatchRule();
}
