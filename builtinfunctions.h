#pragma once

#include <memory>

using namespace std;

class Flame;

void print_str(shared_ptr<Flame>);
void print_int(shared_ptr<Flame>);
void print_double(shared_ptr<Flame>);
void print_bool(shared_ptr<Flame>);
void abs_int(shared_ptr<Flame>);
void int2double(shared_ptr<Flame>);
void double2int(shared_ptr<Flame>);
void rand_int(shared_ptr<Flame>);
void pow_int(shared_ptr<Flame>);
void length_str(shared_ptr<Flame>);
void glut_openwindow(shared_ptr<Flame>);
void glut_setdispfunc(shared_ptr<Flame>);
void glut_setkeyboardfunc(shared_ptr<Flame>);
void glut_setmousefunc(shared_ptr<Flame>);
void glut_mainloop(shared_ptr<Flame>);
void glut_clear(shared_ptr<Flame>);
void glut_char(shared_ptr<Flame>);
void glut_begin_point(shared_ptr<Flame>);
void glut_begin_line(shared_ptr<Flame>);
void glut_begin_strip(shared_ptr<Flame>);
void glut_begin_lineloop(shared_ptr<Flame>);
void glut_begin_triangle(shared_ptr<Flame>);
void glut_begin_quad(shared_ptr<Flame>);
void glut_begin_trianglefan(shared_ptr<Flame>);
void glut_begin_polygon(shared_ptr<Flame>);
void glut_flush(shared_ptr<Flame>);
void glut_end(shared_ptr<Flame>);
void glut_vertex2i(shared_ptr<Flame>);
void glut_color3i(shared_ptr<Flame>);

void math_sin(shared_ptr<Flame>);
void math_cos(shared_ptr<Flame>);
void math_tan(shared_ptr<Flame>);
void math_asin(shared_ptr<Flame>);
void math_acos(shared_ptr<Flame>);
void math_atan(shared_ptr<Flame>);
void math_sqrt(shared_ptr<Flame>);

void sleep_msec(shared_ptr<Flame>);
