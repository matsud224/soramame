#include "builtinfunctions.h"
#include <cmath>
#include <string>
#include <iostream>
#include <cstdlib>
#include "vm.h"
#include "compiler.h"
#include "basic_object.h"
#include "ast.h"
#include <GL/glut.h>
#include <memory>


#define VM_STACK_POP vmptr->Environment.back()->OperandStack.pop()
#define VM_STACK_PUSH(x) vmptr->Environment.back()->OperandStack.push((x))
#define VM_STACK_GET vmptr->Environment.back()->OperandStack.top()
#define VM_OPERAND_GET (*(vmptr->Environment.back()->CodePtr))[vmptr->Environment.back()->PC++]

using namespace std;


//**************GLUT用のいろいろ*******************
int glut_dispfunid;
int glut_keyboardfunid;
int glut_mousefunid;
shared_ptr<VM> glut_vmptr;
void display(){
	/*shared_ptr<ClosureObject> cobj=reinterpret_cast<shared_ptr<ClosureObject> >(STACK_GET.ref_value);
	shared_ptr<FunctionAST> callee=reinterpret_cast<shared_FunctionAST> >(GETCONSTANT(cobj->PoolIndex));
	STACK_POP;

	if(callee->isBuiltin){
		//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
		string builtin_name=callee->Name;
		string typestr=callee->TypeInfo->GetName();

		CodeInfo->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](this);

	}else{
		//フレームを作成
		vector< pair<string,int> > *vars=make_shared<vector< pair<string,int> >();
		//引数の準備
		for(int i=callee->Args->size()-1;i>=0;i--){
			(*vars).push_back(pair<string,int>(callee->Args->at(i).first,reinterpret_cast<int>(STACK_GETINT)));
			STACK_POP;
		}
		//ローカル変数の準備
		for(int i=callee->LocalVariables->size()-1;i>=0;i--){
			(*vars).push_back(pair<string,int>(callee->LocalVariables->at(i).first,0)); //ローカル変数はすべて0に初期化される
		}
		shared_ptr<Flame> inv_flame=make_shared<Flame(vars,&(callee->bytecodes),cobj->ParentFlame);
		for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
			//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
			reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetValue((*(callee->ChildPoolIndex))[i]))->ParentFlame=inv_flame;
		}
		if(callee->ChildPoolIndex->size()==0){
			inv_flame->NoChildren=true;
		}
		Environment.push_back(inv_flame);
	}*/

	glut_vmptr->Run(true); //指定された関数のフレームを作成し、実行。そのフレームがポップされた時点で帰ってくる（trueを指定したので）
}

void resize(int w, int h)
{
  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 変換行列の初期化 */
  glLoadIdentity();

  /* スクリーン上の座標系をマウスの座標系に一致させる */
  glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}

void mouse(int button, int state, int x, int y)
{/*
	shared_ptr<ClosureObject> cobj=reinterpret_cast<shared_ptr<ClosureObject> >(STACK_GET.ref_value);
	shared_ptr<FunctionAST> callee=reinterpret_cast<shared_FunctionAST> >(GETCONSTANT(cobj->PoolIndex));
	STACK_POP;

	if(callee->isBuiltin){
		//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
		string builtin_name=callee->Name;
		string typestr=callee->TypeInfo->GetName();

		CodeInfo->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](this);

	}else{
		//フレームを作成
		vector< pair<string,int> > *vars=make_shared<vector< pair<string,int> >();
		//引数の準備
		for(int i=callee->Args->size()-1;i>=0;i--){
			(*vars).push_back(pair<string,int>(callee->Args->at(i).first,reinterpret_cast<int>(STACK_GETINT)));
			STACK_POP;
		}
		//ローカル変数の準備
		for(int i=callee->LocalVariables->size()-1;i>=0;i--){
			(*vars).push_back(pair<string,int>(callee->LocalVariables->at(i).first,0)); //ローカル変数はすべて0に初期化される
		}
		shared_ptr<Flame> inv_flame=make_shared<Flame(vars,&(callee->bytecodes),cobj->ParentFlame);
		for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
			//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
			reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetValue((*(callee->ChildPoolIndex))[i]))->ParentFlame=inv_flame;
		}
		if(callee->ChildPoolIndex->size()==0){
			inv_flame->NoChildren=true;
		}
		Environment.push_back(inv_flame);
	}*/

	glut_vmptr->Run(true); //指定された関数のフレームを作成し、実行。そのフレームがポップされた時点で帰ってくる（trueを指定したので）
}


void keyboard(unsigned char key, int x, int y)
{
/*	ClosureObject *cobj=reinterpret_cast<ClosureObject *>(glut_vmptr->CodeInfo->PublicConstantPool.GetValue(glut_keyboardfunid));
	FunctionAST *callee=reinterpret_cast<FunctionAST *>(glut_vmptr->CodeInfo->PublicConstantPool.GetValue(cobj->PoolIndex));

	//フレームを作成
	vector< pair<string,int> > *vars=make_shared<vector< pair<string,int> >();
	//引数の準備
	glut_vmptr->Environment.back()->OperandStack.push(y);
	glut_vmptr->Environment.back()->OperandStack.push(x);
	glut_vmptr->Environment.back()->OperandStack.push(key);
	for(int i=callee->Args->size()-1;i>=0;i--){
		(*vars).push_back(pair<string,int>(callee->Args->at(i).first,reinterpret_cast<int>(glut_vmptr->Environment.back()->OperandStack.top())));
		glut_vmptr->Environment.back()->OperandStack.pop();
	}
	//ローカル変数の準備
	for(int i=callee->LocalVariables->size()-1;i>=0;i--){
		(*vars).push_back(pair<string,int>(callee->LocalVariables->at(i).first,0)); //ローカル変数はすべて0に初期化される
	}
	Flame *inv_flame=make_shared<Flame(vars,&(callee->bytecodes),cobj->ParentFlame);
	for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
		//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
		reinterpret_cast<FunctionAST *>(glut_vmptr->CodeInfo->PublicConstantPool.GetValue((*(callee->ChildPoolIndex))[i]))->ParentFlame=inv_flame;
	}
	glut_vmptr->Environment.push_back(inv_flame);
*/
	glut_vmptr->Run(true); //指定された関数のフレームを作成し、実行。そのフレームがポップされた時点で帰ってくる（trueを指定したので）
}
//**************GLUT用のいろいろ*******************

void print_str(shared_ptr<VM> vmptr){
	string str=*(static_pointer_cast<string>(VM_STACK_GET.ref_value)); VM_STACK_POP;
	cout<<str<<flush;
}

void print_int(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.int_value; VM_STACK_POP;
	cout<<iopr1<<flush;
}

void print_bool(shared_ptr<VM> vmptr){
	bool bopr1=static_cast<bool>(VM_STACK_GET.int_value); VM_STACK_POP;
	cout<<(bopr1?"true":"false")<<flush;
}

void abs_int(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.int_value; VM_STACK_POP;
	VMValue v;v.int_value=abs(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void rand_int(shared_ptr<VM> vmptr){
	//返り値を直にプッシュ
	VMValue v;v.int_value=rand();
	VM_STACK_PUSH(v);
}

void pow_int(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.int_value; VM_STACK_POP;
	int iopr2=VM_STACK_GET.int_value; VM_STACK_POP;
	VMValue v;v.int_value=pow(iopr1,iopr2);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void length_str(shared_ptr<VM> vmptr){
	string str=*(static_pointer_cast<string>(VM_STACK_GET.ref_value)); VM_STACK_POP;
	VMValue v;v.int_value=str.length();
	VM_STACK_PUSH(v);
}

void glut_openwindow(shared_ptr<VM> vmptr){
	int argc=0;char *argv[0];
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA);
	string str=*(static_pointer_cast<string>(VM_STACK_GET.ref_value)); VM_STACK_POP;
	glutCreateWindow(str.c_str());
	glutReshapeFunc(resize);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void glut_setdispfunc(shared_ptr<VM> vmptr){
	glut_dispfunid=VM_STACK_GET.int_value;VM_STACK_POP;
	glut_vmptr=vmptr;
	glutDisplayFunc(display);
}

void glut_setmousefunc(shared_ptr<VM> vmptr){
	glut_mousefunid=VM_STACK_GET.int_value;VM_STACK_POP;
	glut_vmptr=vmptr;
	glutMouseFunc(mouse);
}

void glut_setkeyboardfunc(shared_ptr<VM> vmptr){
	glut_keyboardfunid=VM_STACK_GET.int_value;VM_STACK_POP;
	glut_vmptr=vmptr;
	glutKeyboardFunc(keyboard);
}

void glut_mainloop(shared_ptr<VM> vmptr){
	glutMainLoop();
}

void glut_clear(shared_ptr<VM> vmptr){
	glClear(GL_COLOR_BUFFER_BIT);
}

void glut_char(shared_ptr<VM> vmptr){
	int x,y;
	x=VM_STACK_GET.int_value;VM_STACK_POP;
	y=VM_STACK_GET.int_value;VM_STACK_POP;
	glRasterPos3i(x, y, 0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,VM_STACK_GET.int_value);VM_STACK_POP;
}

void glut_begin_point(shared_ptr<VM> vmptr){
	glBegin(GL_POINTS);
}

void glut_begin_line(shared_ptr<VM> vmptr){
	glBegin(GL_LINES);
}

void glut_begin_strip(shared_ptr<VM> vmptr){
	glBegin(GL_LINE_STRIP);
}

void glut_begin_lineloop(shared_ptr<VM> vmptr){
	glBegin(GL_LINE_LOOP);
}

void glut_begin_triangle(shared_ptr<VM> vmptr){
	glBegin(GL_TRIANGLES);
}

void glut_begin_quad(shared_ptr<VM> vmptr){
	glBegin(GL_QUADS);
}

void glut_begin_trianglefan(shared_ptr<VM> vmptr){
	glBegin(GL_TRIANGLE_FAN);
}

void glut_begin_polygon(shared_ptr<VM> vmptr){
	glBegin(GL_POLYGON);
}

void glut_flush(shared_ptr<VM> vmptr){
	glFlush();
}

void glut_end(shared_ptr<VM> vmptr){
	glEnd();
}

void glut_vertex2i(shared_ptr<VM> vmptr){
	int x=VM_STACK_GET.int_value;VM_STACK_POP;
	int y=VM_STACK_GET.int_value;VM_STACK_POP;
	glVertex2i(x,y);
}

void glut_color3i(shared_ptr<VM> vmptr){
	int r,g,b;
	r=VM_STACK_GET.int_value;VM_STACK_POP;
	g=VM_STACK_GET.int_value;VM_STACK_POP;
	b=VM_STACK_GET.int_value;VM_STACK_POP;
	//glcolor3iは0~INT_MAXで指定なので使いにくい（２５５までじゃない！！！）
	glColor3ub(r,g,b);
}

