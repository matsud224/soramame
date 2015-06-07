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
	int candidate_i, candidate_len = -1; //最長一致にするため、現在の最長一致を覚えておく（iが小さいほうが優先ー＞上から下へ優先順位が下がっていく）

    //ルールをひとつずつ上から順に試していき，マッチしたら返す
    for(int i=0;i<TOKENRULECOUNT;i++){
        if(TOKENRULE[i].state!=curr_state){
            continue;
        }
        if(regex_search(&(rawcode[curr_index]),match,regex_objs[i])){
            //現在の読み取り開始位置にマッチしたのか？
            if(match.position()!=0){
                //離れた位置でマッチした
                continue;
            }

			if (candidate_len < match.length()){
				candidate_i = i;
				candidate_len = match.length();
			}

        }
    }

	if (candidate_len == -1){
		//マッチするものがなかった
		throw NoMatchRule();
	}


	if (regex_search(&(rawcode[curr_index]), match, regex_objs[candidate_i])){
		curr_index += match.length();

		if (TOKENRULE[candidate_i].hasValue == true){
			return TOKENRULE[candidate_i].callback(match.str(), this);
		}
		else{
			if (TOKENRULE[candidate_i].callback != NULL){
				TOKENRULE[candidate_i].callback(match.str(), this);
			}
			goto re_get;
		}
	}

}
