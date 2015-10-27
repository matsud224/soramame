fun main(){
	var a=4.0,b=3.0
    glut_openwindow("Lissajous")
    glut_setdisplayfunc(fun(){
        glut_clear(); glut_begin_point()
        
        for(0.0,1000.0,1.0,fun(t:double){
            glut_color3i(255,0,0)
            glut_vertex2i(d2i(sin(a*t)*50.0)+100,
                    d2i(cos(b*t)*50.0)+100)
        })
        glut_end(); glut_flush()
  })
  var timerfun:fun(int)=>void =fun(n:int){
        a=a+1.0;b=b+1.0
		glut_postredisp();
		glut_settimerfunc(1000,timerfun,1)
  }
  glut_settimerfunc(1000,timerfun,1)
  glut_mainloop()
}

fun for(start:double,end:double,step:double,block:fun(double)=>void){
    if(start>end){return}
    block(start)
    for(start+step,end,step,block)
}
