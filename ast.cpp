#include <vector>
#include <string>
#include "ast.h"
#include "vm.h"
#include "utility.h"
#include <iostream>
#include <stack>
#include <queue>
#include <typeinfo>


FunctionAST* Parser::ParseFunction()
{
    string fname;
    vector< pair<string,TypeAST *> > args;
    TypeAST *rettype;
    vector<ExprAST *> body;

    if(lexer->CurrentToken!=token_func){
        error("関数宣言でないものがあります。");
    }
    lexer->GetNextToken();

    if(lexer->CurrentToken!=token_identifier){
        error("関数名がありません。");
    }else{
        fname=lexer->CurrentIdentifier;
        if(lexer->GetNextToken(),lexer->CurrentToken!='('){
            error("引数リストの開き括弧がありません。");
        }else{
            //引数リスト取得
            lexer->GetNextToken();
            rettype=new TypeAST("void");
            while(lexer->CurrentToken!=')'){
                string argname;
                if(lexer->CurrentToken!=token_identifier){
                    error("引数名がありません。");
                }else{
                    argname=lexer->CurrentIdentifier;
                    if(lexer->GetNextToken(),lexer->CurrentToken!=':'){
                        error("引数リストのコロンがありません。");
                    }else{
                        lexer->GetNextToken();
                        args.push_back(pair<string,TypeAST *>(argname,ParseType()));
                        if(lexer->CurrentToken==','){
                            lexer->GetNextToken();
                        }
                    }
                }
            }
            if(lexer->GetNextToken(),lexer->CurrentToken==token_operator && lexer->CurrentOperator=="=>"){
                //戻り値の型が指定されている
                lexer->GetNextToken();
                delete rettype;
                rettype=ParseType();
            }
        }
    }

    string thistype="(";
    vector< pair<string,TypeAST *> >::iterator iter;
    bool is_noitem=true;
    for(iter=args.begin();iter!=args.end();iter++){
        thistype+=(*iter).second->GetName()+",";
        is_noitem=false;
    }
    if(!is_noitem){
        thistype.erase( --thistype.end() ); //最後のカンマを削除
    }
    thistype+=")=>"+rettype->GetName();

    cout<<"トップレベルの関数定義　　名前:"<<fname<<" 引数の数:"<<args.size()<<" 型:"<< thistype<<endl;


    if(lexer->CurrentToken!='{'){
        error("関数宣言の開き波括弧がありません。");
    }else{
        lexer->GetNextToken();
        while(lexer->CurrentToken!='}'){
            //return文は特別扱い
            if(lexer->CurrentToken==token_return){
                body.push_back(ParseReturnExpr());
            }else{
                body.push_back(ParsePrimary());
            }
        }
    }
    lexer->GetNextToken();
    return new FunctionAST(&genInfo,fname,args,new TypeAST(thistype),body);
}

TypeAST* Parser::ParseType()
{
    TypeAST *ret;
    if(lexer->CurrentToken=='('){
        //関数型の場合
        string functiontypename="(";
        bool is_noarg=true;
        lexer->GetNextToken();
        while(lexer->CurrentToken!=')'){
            is_noarg=false;
            functiontypename+=ParseType()->GetName()+",";
            if(lexer->CurrentToken==','){
                lexer->GetNextToken();
            }
        }
        if(!is_noarg){
            functiontypename.erase( --functiontypename.end() );
        }
        functiontypename+=")";
        lexer->GetNextToken();
        if(lexer->CurrentToken!=token_operator || lexer->CurrentOperator!="=>"){
            error("関数型の'戻り値の型'の記述がありません。");
        }
        lexer->GetNextToken();
        ret=new TypeAST(functiontypename+"=>"+(ParseType()->GetName()));
    }else{
        ret=new TypeAST(lexer->CurrentIdentifier);
        lexer->GetNextToken();
    }

    return ret;
}

ExprAST* Parser::ParsePrimary()
{
    switch(lexer->CurrentToken){
    case token_intval:
        return ParseIntValExpr();
    case token_floatval:
        return ParseFloatValExpr();
    case token_boolval:
        return ParseBoolValExpr();
    case token_stringval:
        return ParseStringValExpr();
    case token_identifier:
        return ParseIdentifierExpr();
    case '(':
        return ParseParenExpr();
    default:
        error("式がありません。");
    }

    return NULL;
}

ExprAST* Parser::ParseIntValExpr()
{
    IntValExprAST *ret=new IntValExprAST(lexer->CurrentIntVal);
    lexer->GetNextToken();
    return ret;
}

ExprAST* Parser::ParseBoolValExpr()
{
    BoolValExprAST *ret=new BoolValExprAST(lexer->CurrentBoolVal);
    lexer->GetNextToken();
    return ret;
}

ExprAST* Parser::ParseStringValExpr()
{
    StringValExprAST *ret=new StringValExprAST(&genInfo,lexer->CurrentStringVal);
    lexer->GetNextToken();
    return ret;
}

ExprAST* Parser::ParseClosureExpr(){
    TypeAST *rettype;
    vector< pair<string,TypeAST *> > args;
    vector<ExprAST *> body;

    if(lexer->CurrentToken!='('){
        error("クロージャの引数リストの開き括弧がありません。");
    }else{
        //引数リスト取得
        lexer->GetNextToken();
        while(lexer->CurrentToken!=')'){
            string argname;
            if(lexer->CurrentToken!=token_identifier){
                error("クロージャの引数名がありません。");
            }else{
                argname=lexer->CurrentIdentifier;
                if(lexer->GetNextToken(),lexer->CurrentToken!=':'){
                    error("クロージャの引数リストのコロンがありません。");
                }else{
                    lexer->GetNextToken();
                    args.push_back(pair<string,TypeAST *>(argname,ParseType()));
                    if(lexer->CurrentToken==','){
                        lexer->GetNextToken();
                    }
                }
            }
        }

        lexer->GetNextToken();
        if(lexer->CurrentToken==token_operator && lexer->CurrentOperator=="=>"){
            string fname="<closure>";
            lexer->GetNextToken();
            rettype=ParseType();

            string thistype="(";
            vector< pair<string,TypeAST *> >::iterator iter;
            bool is_noitem=true;
            for(iter=args.begin();iter!=args.end();iter++){
                thistype+=(*iter).second->GetName()+",";
                is_noitem=false;
            }
            if(!is_noitem){
                thistype.erase( --thistype.end() ); //最後のカンマを削除
            }
            thistype+=")=>"+rettype->GetName();


            if(lexer->CurrentToken!='{'){
                error("クロージャの開き波括弧がありません。");
            }else{
                lexer->GetNextToken();
                while(lexer->CurrentToken!='}'){
                    //return文は特別扱い
                    if(lexer->CurrentToken==token_return){
                        body.push_back(ParseReturnExpr());
                    }else{
                        body.push_back(ParsePrimary());
                    }
                }
            }
            lexer->GetNextToken();
            return new FunctionAST(&genInfo,fname,args,new TypeAST(thistype),body);
        }else{
            error("'クロージャの戻り値'の型が指定されていません。");
        }
    }

    return NULL;
}

ExprAST* Parser::ParseIdentifierExpr()
{
    string idname=lexer->CurrentIdentifier;
    lexer->GetNextToken();

    if(lexer->CurrentToken!='('){
        //単なる変数
        return new VariableExprAST(idname);
    }else{
        //関数呼び出し
        vector<ExprAST *> args;
        lexer->GetNextToken();
        while(lexer->CurrentToken!=')'){
            args.push_back(ParseExpression());
            if(lexer->CurrentToken==','){
                lexer->GetNextToken();
            }else if(lexer->CurrentToken!=')'){
                error("関数呼び出しの引数リストにカンマまたは閉じ括弧がありません。");
            }
        }
        lexer->GetNextToken();
        return new CallExprAST(idname,args);
    }
}

ExprAST* Parser::ParseFloatValExpr()
{
    FloatValExprAST *ret=new FloatValExprAST(lexer->CurrentFloatVal);
    lexer->GetNextToken();
    return ret;
}

ExprAST* Parser::ParseParenExpr()
{
    //閉じかっこを探し、その後ろが=>ならばクロージャ
    if(lexer->isClosureAfterParen()){
        //クロージャ
        return ParseClosureExpr();
    }

    lexer->GetNextToken();
    ExprAST *v=ParseExpression();

    if(lexer->CurrentToken!=')'){
        error("閉じ括弧が見つかりません。");
    }else{
        lexer->GetNextToken();
        return v;
    }

    return NULL;
}


ExprAST* Parser::ParseExpression()
{
    //式の解析を操車場アルゴリズムで行う
    //一旦RPNに直して、それからASTにする
    queue<ExprAST *> output;
    stack<OperatorAST *> operatorstack;

    while(1){
        if(lexer->CurrentToken==')' || lexer->CurrentToken=='}' || lexer->CurrentToken==';'){
            //式の終端の決め方がよくわからないのでとりあえずこうしておく
            break;
        }else if(lexer->CurrentToken!=token_operator){
            output.push(ParsePrimary());
        }else{
            while(!operatorstack.empty()){
                string op1=lexer->CurrentOperator;
                string op2=operatorstack.top()->GetName();
                if((genInfo.OperatorList[op1]->GetAssociativity()==Left && genInfo.OperatorList[op1]->GetPrecedence()<=genInfo.OperatorList[op2]->GetPrecedence()) || (genInfo.OperatorList[op1]->GetPrecedence()<genInfo.OperatorList[op2]->GetPrecedence())){
                    output.push(new OperatorAST(op2));
                    operatorstack.pop();
                }else{
                    break;
                }
            }
            operatorstack.push(new OperatorAST(lexer->CurrentOperator));
            lexer->GetNextToken();
        }


    }
    while(!operatorstack.empty()){
        output.push(operatorstack.top());
        operatorstack.pop();
    }

    //この時点でoutputにRPN形式で式が入っている
    stack<ExprAST *> calcstack; //スタックを使いRPNを展開していく
    while(!output.empty()){
        calcstack.push(output.front());
        output.pop();
        if(typeid(*(calcstack.top()))==typeid(OperatorAST)){
            OperatorAST *op=dynamic_cast<OperatorAST *>(calcstack.top()); calcstack.pop();
            ExprAST *operand1,*operand2;
            if(genInfo.OperatorList[op->GetName()]->GetUnaryOrBinary()==Binary){
                operand2=calcstack.top(); calcstack.pop();
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new BinaryExprAST(op->GetName(),operand1,operand2)); //マージ
            }else{
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new UnaryExprAST(op->GetName(),operand1)); //マージ
            }
        }
    }

    if(calcstack.size()!=1){
        error("expressionに問題があります。");
    }

    return calcstack.top();
}


void Parser::Parse()
{
    //演算子の準備
    genInfo.OperatorList["+"]=new OperatorInfo(Binary,Left,20);
    genInfo.OperatorList["-"]=new OperatorInfo(Binary,Left,20);
    genInfo.OperatorList["*"]=new OperatorInfo(Binary,Left,40);
    genInfo.OperatorList["/"]=new OperatorInfo(Binary,Left,40);
    genInfo.OperatorList[">>"]=new OperatorInfo(Binary,Left,10);
    genInfo.OperatorList["<<"]=new OperatorInfo(Binary,Left,10);
    genInfo.OperatorList["%"]=new OperatorInfo(Binary,Left,40);
    genInfo.OperatorList["&&"]=new OperatorInfo(Binary,Left,5);
    genInfo.OperatorList["||"]=new OperatorInfo(Binary,Left,5);
    genInfo.OperatorList["!"]=new OperatorInfo(Unary,Right,70);

    genInfo.OperatorList["<"]=new OperatorInfo(Binary,Left,8);
    genInfo.OperatorList[">"]=new OperatorInfo(Binary,Left,8);
    genInfo.OperatorList["<="]=new OperatorInfo(Binary,Left,8);
    genInfo.OperatorList[">="]=new OperatorInfo(Binary,Left,8);
    genInfo.OperatorList["=="]=new OperatorInfo(Binary,Left,6);
    genInfo.OperatorList["!="]=new OperatorInfo(Binary,Left,6);


    vector< pair<string,TypeAST *> > arglist;
    arglist.push_back(pair<string,TypeAST *>("val",new TypeAST("int")));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printint",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(int)=>void")));
    arglist[0]=pair<string,TypeAST *>("val",new TypeAST("float"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printfloat",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(float)=>void")));
    arglist[0]=pair<string,TypeAST *>("val",new TypeAST("bool"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printbool",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(bool)=>void")));
    arglist[0]=pair<string,TypeAST *>("str",new TypeAST("string"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(string)=>void")));


    lexer->GetNextToken();
    while(lexer->CurrentToken!=token_eof){
        genInfo.FunctionList.push_back(ParseFunction());
    }

}


void Parser::TypeCheck()
{
    vector< vector< pair<string,TypeAST *> > *> environment; //現在可視状態にある変数（トップレベルの関数も変数とみなす）のスタック（フレームを積み重ねていく）

    vector<FunctionAST *>::iterator iter;
    vector<ExprAST *>::iterator iter2;

    vector< pair<string,TypeAST *> > *rootflame=new vector< pair<string,TypeAST *> >();
    for(iter=genInfo.FunctionList.begin();iter!=genInfo.FunctionList.end();iter++){
        rootflame->push_back(pair<string,TypeAST *>((*iter)->GetName(),(*iter)->GetType()));
    }
    environment.push_back(rootflame); //トップレベルのフレーム

    for(iter=genInfo.FunctionList.begin();iter!=genInfo.FunctionList.end();iter++){
        (*iter)->CheckType(&environment,&genInfo);
    }
}



void FloatValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    bytecodes->push_back(fpush);
    bytecodes->push_back(value);
    return;
}

void VariableExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    if(type->GetName()=="int"){
        bytecodes->push_back(iloadlocal);
        bytecodes->push_back(localindex);
    }else if(type->GetName()=="float"){
        bytecodes->push_back(floadlocal);
        bytecodes->push_back(localindex);
    }else if(type->GetName()=="bool"){
        bytecodes->push_back(bloadlocal);
        bytecodes->push_back(localindex);
    }else if(type->GetName()=="string"){
        bytecodes->push_back(sloadlocal);
        bytecodes->push_back(localindex);
    }else{
        bytecodes->push_back(rloadlocal);
        bytecodes->push_back(localindex);
    }

    return;
}

void UnaryExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    operand->Codegen(bytecodes,geninfo);
    if(opcode=="!"){
        if(type->GetName()=="bool"){
            bytecodes->push_back(bnot);
        }
    }
    return;
}

void BinaryExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    LHS->Codegen(bytecodes,geninfo);
    RHS->Codegen(bytecodes,geninfo);

    if(opcode=="+"){
        if(type->GetName()=="int"){
            bytecodes->push_back(iadd);
        }else if(type->GetName()=="float"){
            bytecodes->push_back(fadd);
        }
    }else if(opcode=="-"){
        if(type->GetName()=="int"){
            bytecodes->push_back(isub);
        }else if(type->GetName()=="float"){
            bytecodes->push_back(fsub);
        }
    }else if(opcode=="*"){
        if(type->GetName()=="int"){
            bytecodes->push_back(imul);
        }else if(type->GetName()=="float"){
            bytecodes->push_back(fmul);
        }
    }else if(opcode=="/"){
        if(type->GetName()=="int"){
            bytecodes->push_back(idiv);
        }else if(type->GetName()=="float"){
            bytecodes->push_back(fdiv);
        }
    }else if(opcode=="<<"){
        if(type->GetName()=="int"){
            bytecodes->push_back(ilshift);
        }
    }else if(opcode==">>"){
        if(type->GetName()=="int"){
            bytecodes->push_back(irshift);
        }
    }else if(opcode=="%"){
        if(type->GetName()=="int"){
            bytecodes->push_back(imod);
        }
    }else if(opcode=="&&"){
        if(type->GetName()=="bool"){
            bytecodes->push_back(band);
        }
    }else if(opcode=="||"){
        if(type->GetName()=="bool"){
            bytecodes->push_back(bor);
        }
    }else if(opcode=="<"){
        if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmplt);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmplt);
        }
    }else if(opcode==">"){
        if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmpgt);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmpgt);
        }
    }else if(opcode=="<="){
        if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmple);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmple);
        }
    }else if(opcode==">="){
        if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmpge);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmpge);
        }
    }else if(opcode=="=="){
        if(LHS->CheckType(NULL,NULL)->GetName()=="bool"){
            bytecodes->push_back(if_bcmpeq);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmpeq);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmpeq);
        }
    }else if(opcode=="!="){
        if(LHS->CheckType(NULL,NULL)->GetName()=="bool"){
            bytecodes->push_back(if_bcmpne);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="int"){
            bytecodes->push_back(if_icmpne);
        }else if(LHS->CheckType(NULL,NULL)->GetName()=="float"){
            bytecodes->push_back(if_fcmpne);
        }
    }
    return;
}

void CallExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    vector<ExprAST *>::iterator iter;
    for(iter=args.begin();iter!=args.end();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
    bytecodes->push_back(rloadlocal);
    bytecodes->push_back(localindex);
    bytecodes->push_back(invoke);
}


//自身が関数ならばbodyのコード生成を行う。
//自身がクロージャならそれに加えてスタックに自身のポインタを置くコードを書く
void FunctionAST::Codegen(vector<int> *bytecodes_given,CodegenInfo *geninfo)
{
    vector<int> codes;
    vector<ExprAST *>::iterator itere;
    for(itere=body.begin();itere!=body.end();itere++){
        (*itere)->Codegen(&codes,geninfo);
    }

    //最後にreturnを挿入（return文が省略された時のため）
    //戻り値の型で場合分け
    TypeAST *rettype=type->ParseFunctionType().back();;
    if(rettype->GetName()=="int"){
        codes.push_back(iret);
    }else if(rettype->GetName()=="float"){
        codes.push_back(fret);
    }else if(rettype->GetName()=="bool"){
        codes.push_back(bret);
    }else if(rettype->GetName()=="string"){
        codes.push_back(sret);
    }else{
        codes.push_back(rret);
    }

    bytecodes=codes;

    if(name=="<closure>"){
        bytecodes_given->push_back(ldc);
        bytecodes_given->push_back(poolindex);
    }
    return;
}

void IntValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo){
    bytecodes->push_back(ipush);
    bytecodes->push_back(value);
    return;
}

void Parser::Codegen()
{
    vector<FunctionAST *>::iterator iterf;

    for(iterf=genInfo.FunctionList.begin();iterf!=genInfo.FunctionList.end();iterf++){
            //組み込み関数のコード生成は行わない
        if((*iterf)->isBuiltinFunction()==false){
            (*iterf)->Codegen(NULL,&genInfo);
        }
    }

    //main関数（引数なしで、戻り値がvoidであるもの）を探す
    int main_index=-1;
    for(iterf=genInfo.FunctionList.begin();iterf!=genInfo.FunctionList.end();iterf++){
        if((*iterf)->GetName()=="main" && (*iterf)->GetType()->GetName()=="()=>void"){
            main_index=(*iterf)->poolindex;
        }
    }
    if(main_index==-1){
        error("適切なmain関数が見つかりません。");
    }


}

void BoolValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    if(value){
        bytecodes->push_back(btruepush);
    }else{
        bytecodes->push_back(bfalsepush);
    }
}

void StringValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    bytecodes->push_back(ldc);
    bytecodes->push_back(poolindex);
}

ExprAST *Parser::ParseReturnExpr()
{
    lexer->GetNextToken(); //returnを消費
    if(lexer->is_met_semicolon==true || lexer->CurrentToken=='}' || lexer->CurrentToken==')'){
        return new ReturnExpr(NULL);
    }else{
        return new ReturnExpr(ParseExpression());
    }
}

void ReturnExpr::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
    if(type->GetName()=="void"){
        bytecodes->push_back(ret);
    }else if(type->GetName()=="int"){
        bytecodes->push_back(iret);
    }else if(type->GetName()=="float"){
        bytecodes->push_back(fret);
    }else if(type->GetName()=="bool"){
        bytecodes->push_back(bret);
    }else if(type->GetName()=="string"){
        bytecodes->push_back(sret);
    }else{
        bytecodes->push_back(rret);
    }
}

