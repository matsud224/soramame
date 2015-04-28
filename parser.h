#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include "common.h"
#include "lexer.h"
#include "compiler.h"
#include <memory>

using namespace std;



const int MAX_SYNTAXRULE_LEN=20;

class SyntaxRule{
public:
    Symbol rule[MAX_SYNTAXRULE_LEN];
    TokenValue (* callback)(shared_ptr<CodegenInfo>,vector<TokenValue>);
};

extern SyntaxRule SYNTAXRULE[SYNTAXRULECOUNT];

enum SymbolType{TERMINAL,NONTERMINAL};

enum ActionType{SHIFT,REDUCE,ACCEPT};

enum ConflictType{ShiftReduce,ReduceReduce};

class Action{
public:
	ActionType Type;
	int State;

	bool operator<(const Action& obj) const {
		if(obj.Type!=Type){
			return Type<obj.Type;
		}else{
			return State<obj.State;
		}
	}

	Action(ActionType type,int state):Type(type),State(state){}
};

class BacktrackingPoint{
public:
	unsigned int selected_count; //選択したアクションの数（バックトラックするごとに１増える）
	ConflictType Conflict_Type;
	int CodePosition; //lexerを復帰させるために、curr_indexを覚えておく
	State state;
	int curr_line;
    vector<int> StateStack;
    vector< TokenValue > WorkStack;
    pair<Symbol,TokenValue> input;
    pair<Symbol,TokenValue> input_backup;

	//codegeninfoの中身
    vector<shared_ptr<FunctionAST> > TopLevelFunction;
    vector<shared_ptr<VariableDefStatementAST> > TopLevelVariableDef;
    ConstantPool PublicConstantPool;
};

class Parser{
private:
    map< pair<int,Symbol>,int> StateTransitionTable;
    map< pair<int,Symbol>,set<Action> > ActionTable; //conflictのためにActionTypeはvectorにしておく
    map< pair<int,Symbol>,int> GOTOTable;
    vector< shared_ptr<set< pair<Symbol*,int> > > > itemsets;
	set<int> conflict_state;
    set<int> stt_checked; //状態遷移表の作成をしたアイテム集合の番号（2重にチェックするのを避けるため）


    bool is_nonterminal(Symbol s);
    bool is_terminal(Symbol s);
    void BuildStateTable();
    void ProcessItemSet(shared_ptr<set< pair<Symbol*,int> > > current_set,int caller_id,Symbol used_symbol);
    void MakeAction_GotoTable();
    bool isActionExist(int currentstate,Symbol sym);
    int FindItem(pair<Symbol*,int>);
    void TraverseReduces(pair<Symbol*,int>,int,set<int>,vector<int>);
    TokenValue default_action(vector<TokenValue> values);
    void CreateBacktrackingPoint(int,State,int,ConflictType,pair<Symbol,TokenValue>,pair<Symbol,TokenValue>,shared_ptr<CodegenInfo>);

    vector<int> StateStack;
    vector< TokenValue > WorkStack; //データを置いていきます

    vector<BacktrackingPoint> BacktrackingPoint_list;

    bool is_accepted;
public:
	static TokenValue dummy;
	set<int> error_candidates; //バックトラックする度に行番号を記録

    Parser(){
    	is_accepted=false;
        BuildStateTable();

        StateStack.push_back(0);
    }

    void Put(shared_ptr<Lexer>,shared_ptr<CodegenInfo>,pair<Symbol,TokenValue>);

    static string Symbol2Str(Symbol);
    bool IsAccepted(){
		return is_accepted;
    }
};


