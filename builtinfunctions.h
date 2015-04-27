#pragma once

#include <memory>

using namespace std;

class VM;

void print_str(shared_ptr<VM> vmptr);
void print_int(shared_ptr<VM> vmptr);
void print_bool(shared_ptr<VM> vmptr);
void abs_int(shared_ptr<VM> vmptr);
void rand_int(shared_ptr<VM> vmptr);
void pow_int(shared_ptr<VM> vmptr);
void length_str(shared_ptr<VM> vmptr);
void glut_openwindow(shared_ptr<VM> vmptr);
void glut_setdispfunc(shared_ptr<VM> vmptr);
void glut_setkeyboardfunc(shared_ptr<VM> vmptr);
void glut_setmousefunc(shared_ptr<VM> vmptr);
void glut_mainloop(shared_ptr<VM> vmptr);
void glut_clear(shared_ptr<VM> vmptr);
void glut_char(shared_ptr<VM> vmptr);
void glut_begin_point(shared_ptr<VM> vmptr);
void glut_begin_line(shared_ptr<VM> vmptr);
void glut_begin_strip(shared_ptr<VM> vmptr);
void glut_begin_lineloop(shared_ptr<VM> vmptr);
void glut_begin_triangle(shared_ptr<VM> vmptr);
void glut_begin_quad(shared_ptr<VM> vmptr);
void glut_begin_trianglefan(shared_ptr<VM> vmptr);
void glut_begin_polygon(shared_ptr<VM> vmptr);
void glut_flush(shared_ptr<VM> vmptr);
void glut_end(shared_ptr<VM> vmptr);
void glut_vertex2i(shared_ptr<VM> vmptr);
void glut_color3i(shared_ptr<VM> vmptr);
