#pragma once
#include "color_text.h"
#include "vm.h"
#include <iostream>


class Parser;
class Lexer;
class FunctionAST;
class VariableDefStatementAST;
class TypeAST;

enum{Left,Right};
enum{Unary,Binary};

class ConstantPool{
private:
    vector<void *> refpool;
public:
    int SetReference(void *item){
        refpool.push_back(item);
        return refpool.size()-1;
    }
    void *GetReference(int index){
        return refpool[index];
    }
    unsigned int GetPoolSize(){
        return refpool.size();
    }
    void Clear(){
		refpool.clear();
    };
};

class OperatorInfo{
private:
    int unaryorbinary;
    int associativity;
    int precedence;
    //TypeAST lhstype,rhstype,rettype;
public:
    OperatorInfo(int uorb,int assoc,bool is_ctfe=true,int prec=30):unaryorbinary(uorb),associativity(assoc),precedence(prec),IsCTFEable(is_ctfe){}
    int GetAssociativity(){return associativity;}
    int GetPrecedence(){return precedence;}
    int GetUnaryOrBinary(){return unaryorbinary;}
    bool IsCTFEable;
};

//パースした時に得た情報を詰め込んでおく
class CodegenInfo{
public:
    map<string,OperatorInfo *> OperatorList;
    vector<FunctionAST *> TopLevelFunction;
    vector<VariableDefStatementAST *> TopLevelVariableDef;
    vector<int> Bootstrap;
    ConstantPool PublicConstantPool;
    int MainFuncPoolIndex; //main関数のコンスタントプール・インデックス
	vector<int> ChildPoolIndex;
	vector< pair<string,TypeAST*> > LocalVariables;
};

class Compiler{
private:
	void ASTgen();
    void RegisterChildClosure();
    void TypeCheck();
    void CTFE(int);
    void Codegen();
public:
	Lexer *lexer;
	Parser *parser;
    CodegenInfo *genInfo;

    Compiler(Lexer *l,Parser *p):lexer(l),parser(p){
		genInfo=new CodegenInfo();
    }

    void Compile(){
    	cout<<endl<<BG_GREEN"構文解析を行っています..."RESET<<endl;
		ASTgen();
		RegisterChildClosure();
		cout<<BG_GREEN"型検査を行っています..."RESET<<endl;
		TypeCheck();
		cout<<BG_GREEN"コード生成を行っています..."RESET<<endl;
		Codegen();
		cout<<BG_GREEN"コンパイル時関数実行を行っています..."RESET<<endl;
		CTFE(3);

		VM vm(genInfo);
		cout<<BG_BLUE"VMを起動します..."RESET<<endl;
		vm.Run();
		cout<<endl;
    }
};
