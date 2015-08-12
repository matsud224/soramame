data Point{
  x:int
  y:int
}

fun main(){
  //ウィンドウを開く
  glut_openwindow("dragon")
  glut_setdisplayfunc(fun(){
	glut_clear()
	
	paint()
	
	glut_flush()  
  })
  glut_mainloop()
}

fun paint(){

  var P=Point{x=170,y=140};
  var Q=Point{x=400,y=350};


  glut_color3i(0,0,255)
  glut_begin_line()
  drawDragon(P,Q,10)
  glut_end()

}


fun drawDragon(a:Point,b:Point,n:int){

  var c=Point{};

  var xx:int,yy:int;
  xx=b.x-a.x;
  yy=(b.y-a.y)*(1-2);
  

  c.x=a.x+(xx+yy)/2;
  c.y=b.y+(xx+yy)/2;


  if(n<=0){
     drawLine(a.x,a.y,c.x,c.y);
     drawLine(b.x,b.y,c.x,c.y);
  }
  else{
     drawDragon(a,c,n-1);
     drawDragon(b,c,n-1);
  }
} 

fun drawLine(x1:int,y1:int,x2:int,y2:int){
	glut_vertex2i(x1,y1)
	glut_vertex2i(x2,y2)
	//print(x1);print(",");print(y1);print("->");print(x2);print(",");print(y2);print("\n")
}
