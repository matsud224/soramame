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
shared_ptr<ClosureObject> glut_dispfun;
shared_ptr<ClosureObject> glut_keyboardfun;
shared_ptr<ClosureObject> glut_mousefun;

shared_ptr<VM> glut_vmptr;

void display(){
	auto cobj=glut_dispfun;
	auto callee=cobj->FunctionRef;

	if(callee->isBuiltin){
		//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
		string builtin_name=callee->Name;
		string typestr=callee->TypeInfo->GetName();

		glut_vmptr->ExecutableData->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](glut_vmptr);

	}else{
		//フレームを作成
		shared_ptr< vector< pair<string,VMValue> > > vars=make_shared<vector< pair<string,VMValue> > >();

		//ローカル変数の準備
		for(int i=callee->LocalVariables->size()-1;i>=0;i--){
			VMValue v;v.int_value=0;
			(*vars).push_back(pair<string,VMValue>(callee->LocalVariables->at(i).first,v)); //ローカル変数はすべて0に初期化される
		}
		shared_ptr<Flame> inv_flame=make_shared<Flame>(vars,callee->bytecodes,cobj->ParentFlame);
		for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
			//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
			static_pointer_cast<FunctionObject>(glut_vmptr->ExecutableData->PublicConstantPool.GetValue(callee->ChildPoolIndex->at(i)).ref_value)->ParentFlame=inv_flame;
		}
		glut_vmptr->Environment.push_back(inv_flame);
	}

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
{
	auto cobj=glut_mousefun;
	auto callee=cobj->FunctionRef;

	if(callee->isBuiltin){
		//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
		string builtin_name=callee->Name;
		string typestr=callee->TypeInfo->GetName();

		glut_vmptr->ExecutableData->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](glut_vmptr);

	}else{
		//フレームを作成
		shared_ptr< vector< pair<string,VMValue> > > vars=make_shared<vector< pair<string,VMValue> > >();
		//引数の準備
		VMValue v;
		v.int_value=y; (*vars).push_back(pair<string,VMValue>("y",v));
		v.int_value=x; (*vars).push_back(pair<string,VMValue>("x",v));
		v.int_value=state; (*vars).push_back(pair<string,VMValue>("state",v));
		v.int_value=button; (*vars).push_back(pair<string,VMValue>("button",v));

		//ローカル変数の準備
		for(int i=callee->LocalVariables->size()-1;i>=0;i--){
			VMValue v;v.int_value=0;
			(*vars).push_back(pair<string,VMValue>(callee->LocalVariables->at(i).first,v)); //ローカル変数はすべて0に初期化される
		}
		shared_ptr<Flame> inv_flame=make_shared<Flame>(vars,callee->bytecodes,cobj->ParentFlame);
		for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
			//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
			static_pointer_cast<FunctionObject>(glut_vmptr->ExecutableData->PublicConstantPool.GetValue(callee->ChildPoolIndex->at(i)).ref_value)->ParentFlame=inv_flame;
		}
		glut_vmptr->Environment.push_back(inv_flame);
	}

	glut_vmptr->Run(true); //指定された関数のフレームを作成し、実行。そのフレームがポップされた時点で帰ってくる（trueを指定したので）

}


void keyboard(unsigned char key, int x, int y)
{
	auto cobj=glut_keyboardfun;
	auto callee=cobj->FunctionRef;

	if(callee->isBuiltin){
		//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
		string builtin_name=callee->Name;
		string typestr=callee->TypeInfo->GetName();

		glut_vmptr->ExecutableData->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](glut_vmptr);

	}else{
		//フレームを作成
		shared_ptr< vector< pair<string,VMValue> > > vars=make_shared<vector< pair<string,VMValue> > >();
		//引数の準備
		VMValue v;
		v.int_value=y; (*vars).push_back(pair<string,VMValue>("y",v));
		v.int_value=x; (*vars).push_back(pair<string,VMValue>("x",v));
		v.int_value=key; (*vars).push_back(pair<string,VMValue>("key",v));

		//ローカル変数の準備
		for(int i=callee->LocalVariables->size()-1;i>=0;i--){
			VMValue v;v.int_value=0;
			(*vars).push_back(pair<string,VMValue>(callee->LocalVariables->at(i).first,v)); //ローカル変数はすべて0に初期化される
		}
		shared_ptr<Flame> inv_flame=make_shared<Flame>(vars,callee->bytecodes,cobj->ParentFlame);
		for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
			//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
			static_pointer_cast<FunctionObject>(glut_vmptr->ExecutableData->PublicConstantPool.GetValue(callee->ChildPoolIndex->at(i)).ref_value)->ParentFlame=inv_flame;
		}
		glut_vmptr->Environment.push_back(inv_flame);
	}

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

void print_double(shared_ptr<VM> vmptr){
	double dopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	cout<<dopr1<<flush;
}

void print_bool(shared_ptr<VM> vmptr){
	bool bopr1=static_cast<bool>(VM_STACK_GET.bool_value); VM_STACK_POP;
	cout<<(bopr1?"true":"false")<<flush;
}

void abs_int(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.int_value; VM_STACK_POP;
	VMValue v;v.int_value=abs(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void int2double(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.int_value; VM_STACK_POP;
	VMValue v;v.double_value=iopr1;
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void double2int(shared_ptr<VM> vmptr){
	int iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.int_value=iopr1;
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
	glut_dispfun=static_pointer_cast<ClosureObject>(VM_STACK_GET.ref_value);VM_STACK_POP;
	glut_vmptr=vmptr;
	glutDisplayFunc(display);
}

void glut_setmousefunc(shared_ptr<VM> vmptr){
	glut_mousefun=static_pointer_cast<ClosureObject>(VM_STACK_GET.ref_value);VM_STACK_POP;
	glut_vmptr=vmptr;
	glutMouseFunc(mouse);
}

void glut_setkeyboardfunc(shared_ptr<VM> vmptr){
	glut_keyboardfun=static_pointer_cast<ClosureObject>(VM_STACK_GET.ref_value);VM_STACK_POP;
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

void math_sin(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=sin(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_cos(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=cos(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_tan(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=tan(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_asin(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=asin(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_acos(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=acos(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_atan(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=atan(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}

void math_sqrt(shared_ptr<VM> vmptr)
{
	double iopr1=VM_STACK_GET.double_value; VM_STACK_POP;
	VMValue v;v.double_value=sqrt(iopr1);
	//返り値を直にプッシュ
	VM_STACK_PUSH(v);
}
