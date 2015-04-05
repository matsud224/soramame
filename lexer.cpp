#include "oniguruma.h"
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

    OnigRegion *region;

    char *start,*end,*range;
    int r;
    //ルールをひとつずつ上から順に試していき，マッチしたら返す
    for(int i=0;i<TOKENRULECOUNT;i++){
        if(TOKENRULE[i].state!=curr_state){
            continue;
        }
        region=onig_region_new();
        end=const_cast<char*>(rawcode)+strlen(rawcode);
        start=const_cast<char*>(rawcode)+curr_index;
        range=end;
        r=onig_search(regex_objs[i],reinterpret_cast<OnigUChar*>(const_cast<char*>(rawcode)),reinterpret_cast<OnigUChar*>(end),reinterpret_cast<OnigUChar*>(start),reinterpret_cast<OnigUChar*>(range),region,ONIG_OPTION_NONE);
        if(r>=0){
            //現在の読み取り開始位置にマッチしたのか？
            if(r!=curr_index){
                //離れた位置でマッチした
                continue;
            }
            char *str=new char[region->end[0]-region->beg[0]+1]; //NULLのために+1
            for(int k=0;k<region->end[0]-region->beg[0];k++){
                str[k]=rawcode[r+k];
            }
            str[region->end[0]-region->beg[0]]=NULL;

            curr_index+=region->end[0]-region->beg[0];

            onig_region_free(region,1);

            if(TOKENRULE[i].hasValue==true){
				return TOKENRULE[i].callback(str,this);
            }else{
				if(TOKENRULE[i].callback!=NULL){
					TOKENRULE[i].callback(str,this);
				}
				goto re_get;
            }

        }
        onig_region_free(region,1);
    }

    //マッチするものがなかった
    throw NoMatchRule();
}
