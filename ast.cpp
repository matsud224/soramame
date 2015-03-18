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
        error("�֐��錾�łȂ����̂�����܂��B");
    }
    lexer->GetNextToken();

    if(lexer->CurrentToken!=token_identifier){
        error("�֐���������܂���B");
    }else{
        fname=lexer->CurrentIdentifier;
        if(lexer->GetNextToken(),lexer->CurrentToken!='('){
            error("�������X�g�̊J�����ʂ�����܂���B");
        }else{
            //�������X�g�擾
            lexer->GetNextToken();
            rettype=new TypeAST("void");
            while(lexer->CurrentToken!=')'){
                string argname;
                if(lexer->CurrentToken!=token_identifier){
                    error("������������܂���B");
                }else{
                    argname=lexer->CurrentIdentifier;
                    if(lexer->GetNextToken(),lexer->CurrentToken!=':'){
                        error("�������X�g�̃R����������܂���B");
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
                //�߂�l�̌^���w�肳��Ă���
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
        thistype.erase( --thistype.end() ); //�Ō�̃J���}���폜
    }
    thistype+=")=>"+rettype->GetName();

    cout<<" �E�g�b�v���x���̊֐��@�@���O:"<<fname<<" �����̐�:"<<args.size()<<" �^:"<< thistype<<endl;

    vector<int> *childpoolindex=new vector<int>;
    if(lexer->CurrentToken!='{'){
        error("�֐��錾�̊J���g���ʂ�����܂���B");
    }else{
        lexer->GetNextToken();

        while(lexer->CurrentToken!='}'){
            //return���͓��ʈ���
            if(lexer->CurrentToken==token_return){
                body.push_back(ParseReturnExpr(childpoolindex));
            }else if(lexer->CurrentToken==token_var){
            	body.push_back(ParseVariableDefineExpr());
			}else{
                body.push_back(ParsePrimary(childpoolindex));
            }
        }
    }
    lexer->GetNextToken();
    return new FunctionAST(&genInfo,fname,args,new TypeAST(thistype),body,childpoolindex);
}

TypeAST* Parser::ParseType()
{
    TypeAST *ret;
    if(lexer->CurrentToken=='('){
        //�֐��^�̏ꍇ
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
            error("�֐��^��'�߂�l�̌^'�̋L�q������܂���B");
        }
        lexer->GetNextToken();
        ret=new TypeAST(functiontypename+"=>"+(ParseType()->GetName()));
    }else{
        ret=new TypeAST(lexer->CurrentIdentifier);
        lexer->GetNextToken();
    }

    return ret;
}

ExprAST* Parser::ParsePrimary(vector<int> *childpoolindex)
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
        return ParseIdentifierExpr(childpoolindex);
    case '(':
        return ParseParenExpr(childpoolindex);
    default:
        error("��������܂���B");
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

ExprAST* Parser::ParseClosureExpr(vector<int> *childpoolindex){
    TypeAST *rettype;
    vector< pair<string,TypeAST *> > args;
    vector<ExprAST *> body;
    vector<int> *mychildpoolindex=new vector<int>();

    if(lexer->CurrentToken!='('){
        error("�N���[�W���̈������X�g�̊J�����ʂ�����܂���B");
    }else{
        //�������X�g�擾
        lexer->GetNextToken();
        while(lexer->CurrentToken!=')'){
            string argname;
            if(lexer->CurrentToken!=token_identifier){
                error("�N���[�W���̈�����������܂���B");
            }else{
                argname=lexer->CurrentIdentifier;
                if(lexer->GetNextToken(),lexer->CurrentToken!=':'){
                    error("�N���[�W���̈������X�g�̃R����������܂���B");
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
                thistype.erase( --thistype.end() ); //�Ō�̃J���}���폜
            }
            thistype+=")=>"+rettype->GetName();


            if(lexer->CurrentToken!='{'){
                error("�N���[�W���̊J���g���ʂ�����܂���B");
            }else{
                lexer->GetNextToken();
                while(lexer->CurrentToken!='}'){
                    //return���͓��ʈ���
					if(lexer->CurrentToken==token_return){
						body.push_back(ParseReturnExpr(mychildpoolindex));
					}else if(lexer->CurrentToken==token_var){
						body.push_back(ParseVariableDefineExpr());
					}else{
						body.push_back(ParsePrimary(mychildpoolindex));
					}
                }
            }
            lexer->GetNextToken();
            FunctionAST *res=new FunctionAST(&genInfo,fname,args,new TypeAST(thistype),body,mychildpoolindex);
            childpoolindex->push_back(res->poolindex);
            return res;
        }else{
            error("'�N���[�W���̖߂�l'�̌^���w�肳��Ă��܂���B");
        }
    }

    return NULL;
}

ExprAST* Parser::ParseIdentifierExpr(vector<int> *childpoolindex)
{
    string idname=lexer->CurrentIdentifier;
    lexer->GetNextToken();

    if(lexer->CurrentToken!='('){
        //�P�Ȃ�ϐ�
        return new VariableExprAST(idname);
    }else{
        //�֐��Ăяo��
        vector<ExprAST *> args;
        lexer->GetNextToken();
        while(lexer->CurrentToken!=')'){
            args.push_back(ParseExpression(childpoolindex));
            if(lexer->CurrentToken==','){
                lexer->GetNextToken();
            }else if(lexer->CurrentToken!=')'){
                error("�֐��Ăяo���̈������X�g�ɃJ���}�܂��͕����ʂ�����܂���B");
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

ExprAST* Parser::ParseParenExpr(vector<int> *childpoolindex)
{
    //����������T���A���̌�낪=>�Ȃ�΃N���[�W��
    if(lexer->isClosureAfterParen()){
        //�N���[�W��
        return ParseClosureExpr(childpoolindex);
    }

    lexer->GetNextToken();
    ExprAST *v=ParseExpression(childpoolindex);

    if(lexer->CurrentToken!=')'){
        error("�����ʂ�������܂���B");
    }else{
        lexer->GetNextToken();
        return v;
    }

    return NULL;
}

ExprAST* Parser::ParseVariableDefineExpr(){
	vector< pair<string,TypeAST *> > *vars=new vector< pair<string,TypeAST *> >();
	lexer->GetNextToken(); // var������
	while(lexer->CurrentToken==token_identifier){
		string name=lexer->CurrentIdentifier;

		if(lexer->GetNextToken(),lexer->CurrentToken!=':'){
			error("�ϐ���`�ɂ����Č^���w�肳��Ă��܂���B");
		}else{
		    lexer->GetNextToken();
			vars->push_back(pair<string,TypeAST *>(name,ParseType()));
		}
		if(lexer->CurrentToken==','){
            lexer->GetNextToken();
		}else{
            break;
		}
	}
	return new VariableDefineExprAST(vars);
}


ExprAST* Parser::ParseExpression(vector<int> *childpoolindex)
{
    //���̉�͂𑀎ԏ�A���S���Y���ōs��
    //��URPN�ɒ����āA���ꂩ��AST�ɂ���
    queue<ExprAST *> output;
    stack<OperatorAST *> operatorstack;

    while(1){
        if(lexer->CurrentToken==')' || lexer->CurrentToken=='}' || lexer->CurrentToken==';'){
            //���̏I�[�̌��ߕ����悭�킩��Ȃ��̂łƂ肠�����������Ă���
            break;
        }else if(lexer->CurrentToken!=token_operator){
            output.push(ParsePrimary(childpoolindex));
        }else{
            if(genInfo.OperatorList.count(lexer->CurrentOperator)==0){
                //���o�^�̉��Z�q
                error("����`�̉��Z�q�ł�:"+lexer->CurrentOperator);
            }
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

    //���̎��_��output��RPN�`���Ŏ��������Ă���
    stack<ExprAST *> calcstack; //�X�^�b�N���g��RPN��W�J���Ă���
    while(!output.empty()){
        calcstack.push(output.front());
        output.pop();
        if(typeid(*(calcstack.top()))==typeid(OperatorAST)){
            OperatorAST *op=dynamic_cast<OperatorAST *>(calcstack.top()); calcstack.pop();
            ExprAST *operand1,*operand2;
            if(genInfo.OperatorList[op->GetName()]->GetUnaryOrBinary()==Binary){
                operand2=calcstack.top(); calcstack.pop();
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new BinaryExprAST(op->GetName(),operand1,operand2)); //�}�[�W
            }else{
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new UnaryExprAST(op->GetName(),operand1)); //�}�[�W
            }
        }
    }

    if(calcstack.size()!=1){
        error("expression�ɖ�肪����܂��B");
    }

    return calcstack.top();
}


void Parser::Parse()
{
    //���Z�q�̏���
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

	genInfo.OperatorList["="]=new OperatorInfo(Binary,Left,2); //���



    vector< pair<string,TypeAST *> > arglist;
    arglist.push_back(pair<string,TypeAST *>("val",new TypeAST("int")));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printint",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(int)=>void")));
    arglist[0]=pair<string,TypeAST *>("val",new TypeAST("float"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printfloat",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(float)=>void")));
    arglist[0]=pair<string,TypeAST *>("val",new TypeAST("bool"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"printbool",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(bool)=>void")));
    arglist[0]=pair<string,TypeAST *>("str",new TypeAST("string"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(string)=>void")));
    arglist[0]=pair<string,TypeAST *>("val",new TypeAST("int"));
    genInfo.FunctionList.push_back(new FunctionAST(&genInfo,"abs",new vector< pair<string,TypeAST *> >(arglist),new TypeAST("(int)=>int")));

    lexer->GetNextToken();
    while(lexer->CurrentToken!=token_eof){
        genInfo.FunctionList.push_back(ParseFunction());
    }

}


void Parser::TypeCheck()
{
    vector< vector< pair<string,TypeAST *> > *> environment; //���݉���Ԃɂ���ϐ��i�g�b�v���x���̊֐����ϐ��Ƃ݂Ȃ��j�̃X�^�b�N�i�t���[����ςݏd�˂Ă����j

    vector<FunctionAST *>::iterator iter;
    vector<ExprAST *>::iterator iter2;

    vector< pair<string,TypeAST *> > *rootflame=new vector< pair<string,TypeAST *> >();
    for(iter=genInfo.FunctionList.begin();iter!=genInfo.FunctionList.end();iter++){
        rootflame->push_back(pair<string,TypeAST *>((*iter)->GetName(),(*iter)->GetType()));
    }
    environment.push_back(rootflame); //�g�b�v���x���̃t���[��

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
    }else if(opcode=="="){


    }
    return;
}

void CallExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    vector<ExprAST *>::iterator iter;
    for(iter=args.begin();iter!=args.end();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
    if(isBuiltin){
        //�r���g�C���֐��̏ꍇ�́A�t���[������炸�A���ɒl���X�^�b�N�ɒu��
        StringValExprAST *builtin_name=new StringValExprAST(geninfo,callee);
        bytecodes->push_back(ldc);
        bytecodes->push_back(builtin_name->poolindex);
        bytecodes->push_back(invokebuiltin);
    }else{
        bytecodes->push_back(rloadlocal);
        bytecodes->push_back(localindex);
        bytecodes->push_back(invoke);
    }
}


//���g���֐��Ȃ��body�̃R�[�h�������s���B
//���g���N���[�W���Ȃ炻��ɉ����ăX�^�b�N�Ɏ��g�̃|�C���^��u���R�[�h������
void FunctionAST::Codegen(vector<int> *bytecodes_given,CodegenInfo *geninfo)
{
    vector<int> codes;

    vector<ExprAST *>::iterator itere;
    for(itere=body.begin();itere!=body.end();itere++){
        (*itere)->Codegen(&codes,geninfo);
    }

    //�Ō��return��}���ireturn�����ȗ����ꂽ���̂��߁j
    //�߂�l�̌^�ŏꍇ����
    TypeAST *rettype=type->ParseFunctionType().back();;
    if(rettype->GetName()=="void"){
        codes.push_back(ret);
    }else if(rettype->GetName()=="int"){
        codes.push_back(iret);
    }else if(rettype->GetName()=="float"){
        codes.push_back(fret);
    }else if(rettype->GetName()=="bool"){
        codes.push_back(bret);
    }else{
        codes.push_back(rret);
    }

    bytecodes=codes;

    if(name=="<closure>"){
        bytecodes_given->push_back(ldc);
        bytecodes_given->push_back(poolindex);
    }

    string bytecode_names[]={
        "ipush","fpush",
        "btruepush","bfalsepush",
        "iadd","fadd",
        "isub","fsub",
        "imul","fmul",
        "idiv","fdiv",
        "band","bor",
        "imod",
        "ineg","fneg","bnot",
        "ilshift","irshift",
        "iprint","fprint","bprint",
        "if_icmpeq","if_icmpne","if_icmplt","if_icmpgt","if_icmple","if_icmpge",
        "if_fcmpeq","if_fcmpne","if_fcmplt","if_fcmpgt","if_fcmple","if_fcmpge",
        "if_bcmpeq","if_bcmpne",
        "ldc",
        "invoke",
        "iloadlocal","floadlocal","bloadlocal","rloadlocal", //local�Ƃ������O�����A���ǂ̓t���[����k���Ă����̂ŃO���[�o���ϐ��ɍs��������������Ȃ�
        "ret","iret","fret","bret","rret",
        "invokebuiltin" //�r���g�C���֐��Ăяo���i�X�^�b�N�g�b�v�̕����񂪑g�ݍ��݊֐����j

    };
    cout<<endl<<"�֐�:"<<name<<"�̃R�[�h"<<endl;
    for(unsigned int i=0;i<bytecodes.size();i++){
        cout<<bytecodes[i]<<" ("<<bytecode_names[bytecodes[i]]<<")"<<endl;
    }
    cout<<endl;
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
            //�g�ݍ��݊֐��̃R�[�h�����͍s��Ȃ�
        if((*iterf)->isBuiltinFunction()==false){
            (*iterf)->Codegen(NULL,&genInfo);
        }
    }

    //main�֐��i�����Ȃ��ŁA�߂�l��void�ł�����́j��T��
    int main_index=-1;
    for(iterf=genInfo.FunctionList.begin();iterf!=genInfo.FunctionList.end();iterf++){
        if((*iterf)->GetName()=="main" && (*iterf)->GetType()->GetName()=="()=>void"){
            main_index=(*iterf)->poolindex;
        }
    }
    if(main_index==-1){
        error("�K�؂�main�֐���������܂���B");
    }
    genInfo.main_poolindex=main_index;

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

ExprAST *Parser::ParseReturnExpr(vector<int> *childpoolindex)
{
    lexer->GetNextToken(); //return������
    if(lexer->is_met_semicolon==true || lexer->CurrentToken=='}' || lexer->CurrentToken==')'){
        return new ReturnExprAST(NULL);
    }else{
        return new ReturnExprAST(ParseExpression(childpoolindex));
    }
}

void ReturnExprAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
    expression->Codegen(bytecodes,geninfo);

    if(type->GetName()=="void"){
        bytecodes->push_back(ret);
    }else if(type->GetName()=="int"){
        bytecodes->push_back(iret);
    }else if(type->GetName()=="float"){
        bytecodes->push_back(fret);
    }else if(type->GetName()=="bool"){
        bytecodes->push_back(bret);
    }else{
        bytecodes->push_back(rret);
    }
}

