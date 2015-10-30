#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include "parser.h"
#include "common.h"
#include "color_text.h"
#include "utility.h"

using namespace std;


TokenValue Parser::dummy;



string Parser::Symbol2Str(Symbol s){
    switch(s){
	case INPUTEND:
		return "<INPUTEND>";
	case EMPTY:
		return "<EMPTY>";
	case S:
		return "<S>";
	case VAR:
		return "VAR";
	case FUN:
		return "FUN";
	case IF:
		return "IF";
	case ELSE:
		return "ELSE";
	case RETURN_S:
		return "RETURN";
	case INTVAL:
		return "INTVAL";
	case BOOLVAL:
		return "BOOLVAL";
	case STRINGVAL:
		return "STRINGVAL";
	case IDENT:
		return "IDENT";
	case OPERATOR:
		return "OPERATOR";
	case program:
		return "program";
	case functiondef:
		return "functiondef";
	case parameter_list:
		return "parameter_list";
	case parameter:
		return "parameter";
	case variabledef_list:
		return "variabledef_list";
	case variabledef:
		return "variabledef";
	case statement_list:
		return "statement_list";
	case statement:
		return "statement";
	case operator_n:
		return "operator_n";
	case expression:
		return "expression";
	case arg_list:
		return "arg_list";
	case intvalexpr:
		return "intvalexpr";
	case boolvalexpr:
		return "boolvalexpr";
	case stringvalexpr:
		return "stringvalexpr";
	case primary:
		return "primary";
	case variableexpr:
		return "variableexpr";
	case parenexpr:
		return "parenexpr";
	case funcallexpr:
		return "funcallexpr";
	case closureexpr:
		return "closureexpr";
	case returnstatement:
		return "returnstatement";
	case type:
		return "type";
	case type_list:
		return "type_list";
	case ifstatement:
		return "ifstatement";
    case SEMICOLON:
    	return ";";
    case LPAREN:
    	return "(";
    case RPAREN:
    	return ")";
    case LBRACE:
    	return "{";
    case RBRACE:
    	return "}";
	case RBRACKET:
		return "]";
	case LBRACKET:
		return "[";
    case COMMA:
    	return ",";
    case COLON:
    	return ":";
	case DOT:
		return ".";
	case block:
		return "block";
	case WHILE:
		return "WHILE";
	case whilestatement:
		return "whilestatement";
	case listvalexpr:
		return "listvalexpr";
	case DATA:
		return "DATA";
	case datadef:
		return "datadef";
	case GROUP:
		return "GROUP";
	case groupdef:
		return "groupdef";
	case datamember_list:
		return "datamember_list";
	case groupmember_list:
		return "groupmember_list";
	case LINEEND:
		return "<<LINEEND>>";
	case DOUBLEVAL:
		return "DOUBLEVAL";
	case dataexpr:
		return "dataexpr";
	case initassign_list:
		return "initassign_list";
	case listrefexpr:
		return "listrefexpr";
	case datamemberrefexpr:
		return "datamemberrefexpr";
	case doublevalexpr:
		return "doublevalexpr";
    default:
        return "<UNKNOWN SYMBOL :"+IntToString(s)+">";
    }
}


bool Parser::is_nonterminal(Symbol s){
    if(s==INPUTEND){
        return false;
    }
    if(s==EMPTY){
        return false;
    }
    for(int i=0;i<SYNTAXRULECOUNT;i++){
        if(SYNTAXRULE[i].rule[0]==s){
            return true;
        }
    }
    return false;
}

bool Parser::is_terminal(Symbol s){
    if(s==INPUTEND){
        return true;
    }
    if(s==EMPTY){
        return true;
    }
    for(int i=0;i<SYNTAXRULECOUNT;i++){
        if(SYNTAXRULE[i].rule[0]==s){
            return false;
        }
    }
    return true;
}


void Parser::BuildStateTable(){
    ActionTable.clear(); GOTOTable.clear(); StateTransitionTable.clear();
    itemsets.clear(); stt_checked.clear();

    //E → ・ E + B
    //ならば pair<Symbol*,1> とアイテムを表現

    //アイテム集合を作成
    shared_ptr< set< pair<Symbol*,int> > > set0=make_shared<set< pair<Symbol*,int> > >(); //アイテム集合0
    set0->insert(pair<Symbol*,int>(const_cast<Symbol*>(SYNTAXRULE[0].rule),1));
    ProcessItemSet(set0,-1,SYNTAXEND);//（第3引数は適当）

	#ifdef PARSER_DEBUG
    cout<<"アイテム集合の個数："<<itemsets.size()<<endl<<endl;

    cout<<"状態遷移表："<<endl;
    map< pair<int,Symbol>,int>::iterator iter;
    for(iter=StateTransitionTable.begin();iter!=StateTransitionTable.end();iter++){
        cout<<"状態 "<<(*iter).first.first<<" からのシンボル "<<Symbol2Str((*iter).first.second)<<" の時、状態 "<<(*iter).second<<" へ遷移します。"<<endl;
    }
	#endif
    //状態遷移表から構文解析表，GOTO表を作成
    MakeAction_GotoTable();
	map< pair<int,Symbol>,set<Action> >::iterator iter2;

	#ifdef PARSER_DEBUG

    cout<<endl<<"アクション表："<<endl;
    for(iter2=ActionTable.begin();iter2!=ActionTable.end();iter2++){
        set<Action>::iterator iter3;
        for(iter3=(*iter2).second.begin();iter3!=(*iter2).second.end();iter3++){
            switch((*iter3).Type){
            case SHIFT:
                cout<<"<shift> 状態 "<<(*iter2).first.first<<" ,シンボル "<<Symbol2Str((*iter2).first.second)<<" -> 状態"<<(*iter3).State<<endl;
                break;
            case REDUCE:
                cout<<"<reduce> 状態 "<<(*iter2).first.first<<" ,シンボル "<<Symbol2Str((*iter2).first.second)<<" -> 規則"<<(*iter3).State<<endl;
                break;
            case ACCEPT:
                cout<<"<accept> 状態 "<<(*iter2).first.first<<" ,シンボル "<<Symbol2Str((*iter2).first.second)<<endl;
                break;
            default:
                cout<<"無効なアクションです"<<endl;
                break;
            }
        }
    }

    cout<<endl<<"GOTO表："<<endl;
    for(iter=GOTOTable.begin();iter!=GOTOTable.end();iter++){
        cout<<"状態 "<<(*iter).first.first<<" からのシンボル "<<Symbol2Str((*iter).first.second)<<" の時、状態 "<<(*iter).second<<" へ遷移します。"<<endl;
    }

    #endif

    int sr_conflict=0,rr_conflict=0;
    for(iter2=ActionTable.begin();iter2!=ActionTable.end();iter2++){
        if((*iter2).second.size()<2){
            continue;
        }
        set<Action>::iterator iter3;
        int shift=0,reduce=0;
        for(iter3=(*iter2).second.begin();iter3!=(*iter2).second.end();iter3++){
            switch((*iter3).Type){
            case SHIFT:
                shift++;
                break;
            case REDUCE:
                reduce++;
                break;
            }
        }
        if(shift==0){
            rr_conflict++;
        }else{
            sr_conflict++;
        }
    }

	#ifdef PARSER_DEBUG
    cout<<endl<<"shift/reduce conflict: "<<sr_conflict<<endl<<"reduce/reduce conflict: "<<rr_conflict<<endl<<endl;


    if(sr_conflict+rr_conflict>0){
		cout<<"次の状態でコンフリクトが発生しています： ";
		for(set<int>::iterator iter=conflict_state.begin();iter!=conflict_state.end();iter++){
			cout<<*iter<<",";
		}
		cout<<endl<<endl;
    }
	#endif

    return;
}


//再帰的に呼び出され、itemsetへアイテム集合を追加していく。
void Parser::ProcessItemSet(shared_ptr<set< pair<Symbol*,int> > > current_set,int caller_id/*呼び出し元の集合の番号*/,Symbol used_symbol/*状態遷移に際して消費されたシンボル*/){
    set< pair<Symbol*,int> >::iterator iter;

    int before_size;
    do{
        before_size=current_set->size();
        //アイテム集合の拡張を行う（ドットの後ろに非終端子があった場合にその非終端子の構文解析を表すアイテムを追加）
        set< pair<Symbol*,int> > add_reserve; //列挙中に追加するのは危険そうだから
        for(iter=current_set->begin();iter!=current_set->end();iter++){
            if(is_nonterminal( (*iter).first[(*iter).second] )){
                for(int j=0;j<SYNTAXRULECOUNT;j++){
                    if(SYNTAXRULE[j].rule[0]==(*iter).first[(*iter).second]){
                        add_reserve.insert(pair<Symbol*,int>(const_cast<Symbol*>(SYNTAXRULE[j].rule),1));
                    }
                }
            }
        }
        if(add_reserve.size()>0){
            current_set->insert(add_reserve.begin(),add_reserve.end());
        }
    }while(static_cast<unsigned int>(before_size)!=current_set->size());

    //同一の集合があれば、追加しない
    for(unsigned int i=0;i<itemsets.size();i++){
        if(itemsets[i]->size()==current_set->size()){
            if(*(itemsets[i]) == *(current_set)){
                //returnする前に状態遷移表に登録はしておく
                StateTransitionTable[pair<int,Symbol>(caller_id,used_symbol)]=i;
                return;
            }
        }
    }

    int parent_setid=itemsets.size(); //今追加するアイテム集合の番号を控えておく

    #ifdef PARSER_DEBUG
    cout<<"アイテム集合 "<<itemsets.size()<<"が追加されました　個数："<<current_set->size()<<endl;
    for(iter=current_set->begin();iter!=current_set->end();iter++){
        cout<<"   ";
        Symbol *p=(*iter).first;
        int pos=0;
        while(*p!=SYNTAXEND){
            if((*iter).second==pos){
                cout<<"[.] ";
            }
            if(pos==0){
            	cout<<Symbol2Str(*p)<<" -> ";
            }else{
				cout<<Symbol2Str(*p)<<" ";
            }
            p++;pos++;
        }
        if((*iter).second==pos){
            cout<<"[.] ";
        }
        cout<<endl;
    }
    #endif

    itemsets.push_back(current_set);

    if(caller_id!=-1){
        //状態遷移表へ登録
        StateTransitionTable[pair<int,Symbol>(caller_id,used_symbol)]=parent_setid;
    }

    for(int i=-1;i<SYMBOLCOUNT;i++){
        shared_ptr<set< pair<Symbol*,int> > > newset=make_shared<set< pair<Symbol*,int> > >();
        for(iter=current_set->begin();iter!=current_set->end();iter++){
            if((*iter).first[(*iter).second]==i){
                //currnt_setの中で、次のシンボルが、i番目のシンボルと一致する場合
                newset->insert(pair<Symbol*,int>((*iter).first,(*iter).second+1));
            }
        }
        if(!newset->empty()){
            //新たなアイテム集合を作成!
            ProcessItemSet(newset,parent_setid,static_cast<Symbol>(i));

        }else{
            newset.reset();
        }
    }

    return;
}

//与えられたアイテムを含むアイテム集合の番号を返します（見つからないとき-1）
int Parser::FindItem(pair<Symbol*,int> item){
    for(unsigned int i=0;i<itemsets.size();i++){
        if((*itemsets[i]).count(item)==1){
            return i;
        }
    }

    return -1;
}

//アクション表とGOTO表をつくります
void Parser::MakeAction_GotoTable(){
    //非終端記号に関する列はGOTO表に転記
    //終端記号に関する列はアクション表の shift アクションとして転記
    map< pair<int,Symbol>,int>::iterator iter;
    for(iter=StateTransitionTable.begin();iter!=StateTransitionTable.end();iter++){
        if(is_nonterminal((*iter).first.second)){
            GOTOTable[(*iter).first]=(*iter).second;
        }else{
            Action a(SHIFT,(*iter).second);
            ActionTable[(*iter).first].insert(a);
        }
    }

    //入力の終わりを示す 'INPUTEND' の列をアクション表に追加し、アイテム S → E　・　を含むアイテム集合に対応するマスに acc を書き込む
    for(unsigned int i=0;i<itemsets.size();i++){
        if((*itemsets[i]).find(pair<Symbol*,int>(const_cast<Symbol*>(SYNTAXRULE[0].rule),2))!=(*itemsets[i]).end()){
            Action a(ACCEPT,-1);
            ActionTable[pair<int,Symbol>(i,INPUTEND)].insert(a);
        }
    }

    //アクションリストを完成させる
    for(unsigned int i=0;i<itemsets.size();i++){
        set< pair<Symbol*,int> >::iterator iter2;
        for(iter2=(*itemsets[i]).begin();iter2!=(*itemsets[i]).end();iter2++){ //アイテム集合の中身をさがす
            if((*iter2).first[(*iter2).second]==SYNTAXEND){ //A->w*という形式かどうか
                for(int j=0;j<SYNTAXRULECOUNT;j++){
                    if((*iter2).first==SYNTAXRULE[j].rule){
                        if(j>0){
                            for(int s=-2;s<SYMBOLCOUNT;s++){
                                if(is_terminal(static_cast<Symbol>(s))){
                                    Action a(REDUCE,j);
                                    ActionTable[pair<int,Symbol>(i,static_cast<Symbol>(s))].insert(a);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //コンフリクトを可能な限り解決
    // コンフリクトが見つかった`状態’の番号を記録
    map< pair<int,Symbol>,set<Action> >::iterator iter2;
    for(iter2=ActionTable.begin();iter2!=ActionTable.end();iter2++){
        if((*iter2).second.size()>=2){
            conflict_state.insert((*iter2).first.first);
        }
    }

    return;
}

bool Parser::isActionExist(int currentstate,Symbol sym){
    return ActionTable.count(pair<int,Symbol>(currentstate,sym))==1;
}

TokenValue Parser::default_action(vector<TokenValue> values){
	return values[0];
}

//パーサへレキシカルアナライザから入力
void Parser::Put(shared_ptr<Lexer> lexer,shared_ptr<CodegenInfo> cgi,pair<Symbol,TokenValue> input){
	set<Action>::iterator iter;

    pair<Symbol,TokenValue> input_backup; //空規則からの復帰用に入力をとっておくための変数

    #ifdef PARSER_DEBUG
	cout<< CYAN "取得したトークン：" <<Symbol2Str(input.first)<<RESET<<endl;
	#endif

    re_put:
    if(conflict_state.count(StateStack.back())==0 && isActionExist(StateStack.back(),input.first)==false){
        if(input.first==LINEEND){
			return; //次のトークンをもらう（LINEENDを読み飛ばす）
        }

        //対応するアクションが見つからない時は空規則を疑う
        if(input.first!=EMPTY){
            input_backup=input;
            input.first=EMPTY;
            goto re_put;
        }

        //復帰処理　または　構文エラーを吐く
		retry_backtracking:
		if(BacktrackingPoint_list.size()==0){
			throw SyntaxError();
		}else{
			throw SyntaxError();
			#ifdef PARSER_DEBUG
			cout<<BG_MAGENTA"バックトラックします"<<RESET<<endl;
			#endif

			error_candidates.insert(lexer->curr_line);

			BacktrackingPoint btp=BacktrackingPoint_list.back();


			lexer->curr_line=btp.curr_line;
			StateStack=btp.StateStack;
			WorkStack=btp.WorkStack;
			input=btp.input;
			input_backup=btp.input_backup;
			cgi->TopLevelFunction=btp.TopLevelFunction;
			cgi->TopLevelVariableDef=btp.TopLevelVariableDef;
			if(btp.Conflict_Type==ShiftReduce){
				if(ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].size()<=btp.selected_count){
					//これ以上バックトラックできない
					BacktrackingPoint_list.pop_back();
					goto retry_backtracking;
				}
				//shift/reduce
				unsigned int counter=0;
				for(iter=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();iter!=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].end();iter++){
					if((*iter).Type==REDUCE){
						//reduceアクションを選ぶ(一番初めにshift選んだからもうreduceしか残っていない)
						counter++;
						if(counter==btp.selected_count){
							break;
						}
					}
				}
			}else{
				if(ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].size()<=btp.selected_count){
					//これ以上バックトラックできない
					BacktrackingPoint_list.pop_back();
					goto retry_backtracking;
				}
				//reduce/reduce
				unsigned int counter=0;
				for(iter=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();iter!=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].end();iter++){
					//reduceアクションを選ぶ(一番初めにshift選んだからもうreduceしか残っていない)
					counter++;
					if(counter==btp.selected_count+1){
						break;
					}
				}
			}
			btp.selected_count++;
			BacktrackingPoint_list.pop_back();
			BacktrackingPoint_list.push_back(btp); //カウントを１増やして再プッシュ
			goto action_do;
		}
    }


    if(conflict_state.count(StateStack.back())==1){ //現在の状態がコンフリクトを起こしているものなら、動的にアクションリストを構築
		#ifdef PARSER_DEBUG
		cout<<BG_MAGENTA"パース時のコンフリクト解決を試みています....."<<RESET<<endl;
		#endif

        for(int i=-2;i<SYMBOLCOUNT;i++){
            //状態(*c_iter)番 のアクションをすべて消す
            if(is_terminal(static_cast<Symbol>(i))){
				ActionTable.erase(pair<int,Symbol>(StateStack.back(),static_cast<Symbol>(i)));
            }
        }

        set< pair<Symbol*,int> >::iterator iter8;
        for(iter8=(*itemsets[StateStack.back()]).begin();iter8!=(*itemsets[StateStack.back()]).end();iter8++){
            if((*iter8).first[(*iter8).second]==SYNTAXEND){
                //reduceを要求するアイテム

				//reduceに使用される規則を検索
				int apply_rule=-1;
				for(int j=0;j<SYNTAXRULECOUNT;j++){
					if((*iter8).first==SYNTAXRULE[j].rule){
						apply_rule=j; //'適用する規則'の番号
						break;
					}
				}

				set<int> history;
				history.insert(StateStack.back());
                TraverseReduces(*iter8,apply_rule,history,StateStack);

            }else{
                //shiftを要求するアイテム
                pair<int,Symbol> p=pair<int,Symbol>(StateStack.back(),(*iter8).first[(*iter8).second]);
                Action a(SHIFT,FindItem(pair<Symbol*,int>((*iter8).first,(*iter8).second+1)));
                ActionTable[p].insert(a);
                #ifdef PARSER_DEBUG
			    cout<<"<shift> 状態 "<<p.first<<" ,シンボル "<<Symbol2Str(p.second)<<" -> 状態"<<a.State<<endl;
				#endif

            }
        }

        if(ActionTable.count(pair<int,Symbol>(StateStack.back(),input.first))==0){
            if(input.first==LINEEND){
				return; //次のトークンをもらう（LINEENDを読み飛ばす）
			}

            //対応するアクションが見つからない時は空規則を疑う
            if(input.first!=EMPTY){
                input_backup=input;
                input.first=EMPTY;
                goto re_put;
            }

            //復帰処理　または　構文エラーを吐く
            retry_backtracking2:
            if(BacktrackingPoint_list.size()==0){
				throw SyntaxError();
			}else{
				throw SyntaxError();
				#ifdef PARSER_DEBUG
				cout<<BG_MAGENTA"バックトラックします"<<RESET<<endl;
				#endif

				error_candidates.insert(lexer->curr_line);
				BacktrackingPoint btp=BacktrackingPoint_list.back();

				lexer->curr_line=btp.curr_line;
				StateStack=btp.StateStack;
				WorkStack=btp.WorkStack;
				input=btp.input;
				input_backup=btp.input_backup;
				cgi->TopLevelFunction=btp.TopLevelFunction;
				cgi->TopLevelVariableDef=btp.TopLevelVariableDef;
				if(btp.Conflict_Type==ShiftReduce){
					if(ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].size()<=btp.selected_count){
						//これ以上バックトラックできない
						BacktrackingPoint_list.pop_back();
						goto retry_backtracking2;
					}
					//shift/reduce
					unsigned int counter=0;
					for(iter=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();iter!=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].end();iter++){
						if((*iter).Type==REDUCE){
							//reduceアクションを選ぶ(一番初めにshift選んだからもうreduceしか残っていない)
							counter++;
							if(counter==btp.selected_count){
								break;
							}
						}
					}
				}else{
					if(ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].size()<=btp.selected_count){
						//これ以上バックトラックできない
						BacktrackingPoint_list.pop_back();
						goto retry_backtracking2;
					}
					//reduce/reduce
					unsigned int counter=0;
					for(iter=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();iter!=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].end();iter++){
						//reduceアクションを選ぶ(一番初めにshift選んだからもうreduceしか残っていない)
						counter++;
						if(counter==btp.selected_count+1){
							break;
						}
					}
				}
				btp.selected_count++;
				BacktrackingPoint_list.pop_back();
				BacktrackingPoint_list.push_back(btp); //カウントを１増やして再プッシュ
				goto action_do;
			}

        }

    }

    if(ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].size()>=2){
    	//コンフリクトが残ったまま
    	//（コンフリクトは複数（３つ以上も）であることもあるが、shiftは絶対１つ）

		//shiftを優先する
		bool shift_existence=false;
		for(iter=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();iter!=ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].end();iter++){
			if((*iter).Type==SHIFT){
				shift_existence=true;
				break;
			}
		}
		if(shift_existence==false){
			iter = ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();
		}
		#ifdef PARSER_DEBUG
			cout<<BG_MAGENTA"コンフリクトを解決できないので１つ目のアクションを試します"<<RESET<<endl;
		#endif

		CreateBacktrackingPoint(lexer->curr_line,shift_existence?ShiftReduce:ReduceReduce,input,input_backup,cgi);

    }

    //通常はここでアクションをiterにセット
    iter = ActionTable[pair<int,Symbol>(StateStack.back(),input.first)].begin();

	action_do:
    switch((*iter).Type){
    case SHIFT:
        {
            StateStack.push_back((*iter).State);
            WorkStack.push_back(input.second);
            #ifdef PARSER_DEBUG
			cout<<"shiftされました：　状態 "<<(*iter).State<<" へ遷移しました。[";
            for(unsigned int i=0;i<StateStack.size();i++){cout<<StateStack[i]<<",";}
            cout<<"]"<<endl;
			#endif

            if(input.first==EMPTY){
                input=input_backup;
                goto re_put;
            }
            return;
        }
    case REDUCE:
        {
            int rulelen=0;
            shared_ptr<vector<TokenValue> > lval=make_shared<vector<TokenValue> >();
            Symbol *ptr=const_cast<Symbol*>(SYNTAXRULE[(*iter).State].rule)+1; //state番目の規則へのポインタ(右辺を指すため+1する)
            while(*ptr!=SYNTAXEND){
                rulelen++; ptr++;
            }
            for(int i=0;i<rulelen;i++){
                StateStack.pop_back();
                lval->insert(lval->begin(),WorkStack.back()); //自然な順序にするため，先頭に要素を追加していく
                WorkStack.pop_back();
            }
            //GOTOひょうをみる
            StateStack.push_back(GOTOTable[pair<int,Symbol>(StateStack.back(),SYNTAXRULE[(*iter).State].rule[0])]);

            if(SYNTAXRULE[(*iter).State].callback!=NULL){
            	WorkStack.push_back(SYNTAXRULE[(*iter).State].callback(cgi,*lval));
			}else{
				//yaccでいう { $$ = $1; } のこと
				WorkStack.push_back(default_action(*lval));
			}

			#ifdef PARSER_DEBUG
			cout<<"reduceされました：　状態 "<<StateStack.back()<<" へ遷移しました。[";
            for(unsigned int i=0;i<StateStack.size();i++){cout<<StateStack[i]<<",";}
            cout<<"]"<<endl;
			#endif


            if(input.first==EMPTY){
                input=input_backup;
            }
            goto re_put; //reduceでは入力トークンは消費されない
        }
		break;
    case ACCEPT:
        {
        	is_accepted=true;
            if(SYNTAXRULE[0].callback!=NULL){
                vector<TokenValue> v;
                v.push_back(WorkStack.back());
                SYNTAXRULE[0].callback(cgi,v);
            }
            return;
        }
    }

    return;
}

void Parser::CreateBacktrackingPoint(int curr_line,ConflictType conflict_type,pair<Symbol,TokenValue> input,pair<Symbol,TokenValue> input_backup,shared_ptr<CodegenInfo> cgi){
	BacktrackingPoint p;
	p.selected_count=1;
	p.curr_line=curr_line;
	p.Conflict_Type=conflict_type;
	p.StateStack=StateStack;
	p.WorkStack=WorkStack;
	p.input=input;
	p.input_backup=input_backup;
	p.TopLevelFunction=cgi->TopLevelFunction;
	p.TopLevelVariableDef=cgi->TopLevelVariableDef;
	//p.PublicConstantPool=cgi->PublicConstantPool;
	BacktrackingPoint_list.push_back(p);
}

//再帰的に呼び出され、reduceの連鎖をたどりながらreduceすべき次のトークンをアクションリストへ追加していく
void Parser::TraverseReduces(pair<Symbol*,int> selected_item,int origin_apply_rule/*いくらreduce連鎖が深かったとしても初めに適用するルールは一緒*/,set<int> checked_state,vector<int> current_statestack)
{
	//reduceに使用される規則を検索
	int apply_rule=-1;
	for(int j=0;j<SYNTAXRULECOUNT;j++){
		if(selected_item.first==SYNTAXRULE[j].rule){
			apply_rule=j; //'適用する規則'の番号
			break;
		}
	}

    int next_state,rule_len=0; Symbol *ptr=SYNTAXRULE[apply_rule].rule;
    vector<int> applied_statestack=current_statestack; //とりあえずコピー

    while(*ptr!=SYNTAXEND){
        rule_len++;ptr++;
    }
    if(!(StateStack.size()-1-(rule_len-1)>=0 && StateStack.size()-1-(rule_len-1)<=StateStack.size()-1)){
        return;
    }else{
    	for(int i=0;i<rule_len-1;i++){
			applied_statestack.pop_back();
    	}
        next_state=GOTOTable[pair<int,Symbol>(applied_statestack.back(), SYNTAXRULE[apply_rule].rule[0])]; //reduceした後遷移する状態
        applied_statestack.push_back(next_state);
    }

    if(checked_state.count(next_state)==1){
		return;
    }

    set< pair<Symbol*,int> >::iterator iter5;
    set<Symbol> can_put;
    for(iter5=(*itemsets[next_state]).begin();iter5!=(*itemsets[next_state]).end();iter5++){
        //遷移先の状態、アイテム集合から、次に来れるシンボルを列挙
        if((*iter5).first[(*iter5).second]!=SYNTAXEND){
            can_put.insert((*iter5).first[(*iter5).second]);
        }
    }
    set<Symbol>::iterator iter6;
    for(iter6=can_put.begin();iter6!=can_put.end();iter6++){
        if(is_terminal(*iter6)){
            pair<int,Symbol> p=pair<int,Symbol>(StateStack.back(),*iter6);
            Action a(REDUCE,origin_apply_rule);
            ActionTable[p].insert(a);
            #ifdef PARSER_DEBUG
            cout<<"<reduce> 状態 "<<p.first<<" ,シンボル "<<Symbol2Str(p.second)<<" -> 規則"<<a.State<<endl;
            #endif
        }
    }

    checked_state.insert(next_state);

    //nextstateの中を検索
    set< pair<Symbol*,int> >::iterator iter8;
    for(iter8=(*itemsets[next_state]).begin();iter8!=(*itemsets[next_state]).end();iter8++){
        if((*iter8).first[(*iter8).second]==SYNTAXEND){
            //reduceを要求するアイテム
            if((*iter8).first==SYNTAXRULE[0].rule){
                //S -> E ・ の時（規則0）の時は特別
                pair<int,Symbol> p=pair<int,Symbol>(StateStack.back(),INPUTEND);
                Action a(REDUCE,origin_apply_rule);
                ActionTable[p].insert(a);
                #ifdef PARSER_DEBUG
                cout<<"<reduce> 状態 "<<p.first<<" ,シンボル "<<Symbol2Str(p.second)<<" -> 規則"<<a.State<<endl;
                #endif
            }else{
                TraverseReduces(*iter8,origin_apply_rule,checked_state,applied_statestack);
            }
        }
    }
}

