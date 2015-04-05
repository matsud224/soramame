#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include "common.h"
#include "lexer.h"

using namespace std;

class CodegenInfo;

const int MAX_SYNTAXRULE_LEN=20;

struct SyntaxRule{
    Symbol rule[MAX_SYNTAXRULE_LEN];
    TokenValue (* callback)(CodegenInfo*,vector<TokenValue>);
};

extern SyntaxRule SYNTAXRULE[SYNTAXRULECOUNT];

enum SymbolType{TERMINAL,NONTERMINAL};

enum ActionType{SHIFT,REDUCE,ACCEPT};

enum ConflictType{ShiftReduce,ReduceReduce};

struct Action{
	ActionType Type;
	int State;

	bool operator<(const Action& obj) const {
		if(obj.Type!=Type){
			return Type<obj.Type;
		}else{
			return State<obj.State;
		}
	}
};

struct BacktrackingPoint{
public:
	unsigned int selected_count; //選択したアクションの数（バックトラックするごとに１増える）
	ConflictType Conflict_Type;
	int CodePosition; //lexerを復帰させるために、curr_indexを覚えておく
	State state;
	int curr_line;
    vector<int> StateStack;
    stack< TokenValue > WorkStack;
    pair<Symbol,TokenValue> input;
    pair<Symbol,TokenValue> input_backup;
};

class Parser{
private:
    map< pair<int,Symbol>,int> StateTransitionTable;
    map< pair<int,Symbol>,set<Action> > ActionTable; //conflictのためにActionTypeはvectorにしておく
    map< pair<int,Symbol>,int> GOTOTable;
    vector< set< pair<Symbol*,int> > *> itemsets;
	set<int> conflict_state;
    set<int> stt_checked; //状態遷移表の作成をしたアイテム集合の番号（2重にチェックするのを避けるため）

    bool is_nonterminal(Symbol s);
    bool is_terminal(Symbol s);
    void BuildStateTable();
    void ProcessItemSet(set< pair<Symbol*,int> > *current_set,int caller_id,Symbol used_symbol);
    void MakeAction_GotoTable();
    bool isActionExist(int currentstate,Symbol sym);
    int FindItem(pair<Symbol*,int>);
    void TraverseReduces(pair<Symbol*,int>,int,set<int>,vector<int>);
    TokenValue default_action(vector<TokenValue> values);
    void CreateBacktrackingPoint(int,State,int,ConflictType,pair<Symbol,TokenValue>,pair<Symbol,TokenValue>);

    vector<int> StateStack;
    stack< TokenValue > WorkStack; //データを置いていきます

    stack<BacktrackingPoint> BacktrackingPoint_list;

    bool is_accepted;
public:
	static TokenValue dummy;

    Parser(){
    	is_accepted=false;
        BuildStateTable();

        StateStack.push_back(0);
    }

    void Put(Lexer*,CodegenInfo*,pair<Symbol,TokenValue>);

    static string Symbol2Str(Symbol);
    bool IsAccepted(){
		return is_accepted;
    }
};

class ParserException{

};

class SyntaxError : public ParserException{

};
