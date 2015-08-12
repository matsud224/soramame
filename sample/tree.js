data Point{
  x:int
  y:int
}

fun main(){
  glut_openwindow("tree")
  glut_setdisplayfunc(fun(){
	glut_clear()
	
	paint()
	
	glut_flush()  
  })
  glut_mainloop()
}

fun paint(){

  var P=Point{x=100,y=400}
  var Q=Point{x=100,y=100}
  var R=Point{x=250,y=400}
  var S=Point{x=250,y=100}
  var T=Point{x=400,y=400}
  var U=Point{x=400,y=100}

  glut_color3i(20,230,10)
  glut_begin_line()
  drawTree(P,Q,3)
  drawTree(R,S,4)
  drawTree(T,U,5)
  glut_end()
}


fun drawTree(a:Point,b:Point,n:int){

  var STEM_RATIO=0.25,BRANCH_RATIO=0.6
  var PI=3.1415926535
  
  var c=Point{}
  var d=Point{}
  var e=Point{}

  var sign:int;
  var xx:int,yy:int;
  var angle1:double,angle2:double,center_length:double,branch_length:double;

  xx=b.x-a.x;
  yy= b.y-a.y; yy= yy*(-1);

  angle1=atan(i2d(yy)/i2d(xx))+PI/6.0;
  angle2=atan(i2d(yy)/i2d(xx))-PI/6.0;

  center_length=sqrt(i2d(xx*xx+yy*yy))*(1.0-STEM_RATIO);
  branch_length=BRANCH_RATIO*center_length;

  if(xx>=0){sign=1}else{sign=(-1)}
  
  c.x=d2i(i2d(a.x)+STEM_RATIO*i2d(xx))
  c.y=d2i(i2d(a.y)-STEM_RATIO*i2d(yy))
  d.x=c.x+sign*d2i(branch_length*cos(angle1))
  d.y=c.y-sign*d2i(branch_length*sin(angle1))
  e.x=c.x+sign*d2i(branch_length*cos(angle2))
  e.y=c.y-sign*d2i(branch_length*sin(angle2))

 drawLine(a.x,a.y,c.x,c.y);


 if(n<=0){
     drawLine(c.x,c.y,b.x,b.y);
     drawLine(c.x,c.y,d.x,d.y);
     drawLine(c.x,c.y,e.x,e.y);
  }
  else{
     drawTree(c,b,n-1); 
     drawTree(c,d,n-1); 
     drawTree(c,e,n-1); 
  }
} 

fun drawLine(x1:int,y1:int,x2:int,y2:int){
	glut_vertex2i(x1,y1)
	glut_vertex2i(x2,y2)
}
