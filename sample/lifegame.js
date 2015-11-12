var XSIZE:int=50,YSIZE:int=50,BLOCKSIZE:int=15
var XOFFSET:int=10,YOFFSET:int=70
var SPEED:int=500,RUNNING:bool=false
var FIELD:vector(vector(bool))

fun makefield()=>vector(vector(bool)){
	var f=newvector(vector(bool),XSIZE)
	for(0,XSIZE-1,fun(x:int){
		f[x]=newvector(bool,YSIZE)
		for(0,YSIZE-1,fun(y:int){
			f[x][y]=false
		})
	})
	return f
}

fun nextstate(){
	var safe_get:fun(int,int)=>int =fun(x:int,y:int)=>int{
		if(x<0||x>=XSIZE||y<0||y>=YSIZE){
			if(x<0){x=XSIZE-1}	
			if(y<0){y=YSIZE-1}
			if(x>=XSIZE){x=0}
			if(y>=YSIZE){y=0}
			return safe_get(x,y)
		}else{
			if(FIELD[x][y]){return 1}else{return 0}
		}
	}
	var newfield=makefield()
	for(0,XSIZE-1,fun(x:int){
		for(0,YSIZE-1,fun(y:int){
			var livecount=safe_get(x-1,y-1)+safe_get(x,y-1)+safe_get(x+1,y-1)+safe_get(x-1,y)+safe_get(x+1,y)+safe_get(x-1,y+1)+safe_get(x,y+1)+safe_get(x+1,y+1)
			if(FIELD[x][y]){
				if(livecount<=1 || livecount>=4){newfield[x][y]=false}
				else{newfield[x][y]=true}
			}else{
				if(livecount==3){newfield[x][y]=true}
			}
		})
	})
	FIELD=newfield
	glut_postredisp()
}

fun FillRectangle(x:int,y:int,w:int,h:int){
	glut_begin_quad()
	glut_vertex2i(x,y);
	glut_vertex2i(x,y+h);
	glut_vertex2i(x+w,y+h);
	glut_vertex2i(x+w,y);
	glut_end()
}

fun main(){
	var bm=new_buttonmanager()
	var lm=new_labelmanager()
	var sm=new_spinmanager()

	FIELD=makefield()
	
	var timerfun:fun(int)=>void =fun(n:int){
		    nextstate()
			if(RUNNING){glut_settimerfunc(SPEED,timerfun,1)}
	  }

	bm.add(button{text=['S','t','a','r','t'],callback=fun(){if(!RUNNING){glut_settimerfunc(SPEED,timerfun,1)} RUNNING=true},x=10,y=10,marginx=20,marginy=20,height=40,width=100})
	bm.add(button{text=['S','t','o','p'],callback=fun(){RUNNING=false},x=120,y=10,marginx=20,marginy=20,height=40,width=100})
	lm.add(label{text=['S','p','e','e','d'],x=260,y=30})
	bm.add(button{text=['N','e','x','t'],callback=fun(){nextstate()},x=500,y=10,marginx=20,marginy=20,height=40,width=100})
	bm.add(button{text=['C','l','e','a','r'],callback=fun(){
		var newfield=makefield();
		for(0,XSIZE-1,fun(x:int){
			for(0,YSIZE-1,fun(y:int){
				newfield[x][y]=false
			})
		})
		FIELD=newfield;glut_postredisp()
	},x=610,y=10,marginx=20,marginy=20,height=40,width=100})
	sm.add(spin{callback=fun(b:bool){if(b){SPEED=SPEED-100;if(SPEED<0){SPEED=1}}else{SPEED=SPEED+100}},x=320,y=10,marginx=7,marginy=20})
	glut_openwindow("LIFEGAME")
	glut_setdisplayfunc(fun(){
		glut_clear()
		bm.draw()
		lm.draw()
		sm.draw()

		glut_color3i(128,128,128)
		glut_begin_line()
		for(0,YSIZE,fun(y:int){
			glut_vertex2i(0+XOFFSET,y*BLOCKSIZE+YOFFSET);glut_vertex2i(XSIZE*BLOCKSIZE+XOFFSET,y*BLOCKSIZE+YOFFSET)
		})
		for(0,XSIZE,fun(x:int){
			glut_vertex2i(x*BLOCKSIZE+XOFFSET,0+YOFFSET);glut_vertex2i(x*BLOCKSIZE+XOFFSET,YSIZE*BLOCKSIZE+YOFFSET)
		})
		glut_end()
		glut_color3i(0,0,0)
		for(0,XSIZE-1,fun(x:int){
			for(0,YSIZE-1,fun(y:int){
				if(FIELD[x][y]){
					FillRectangle(x*BLOCKSIZE+XOFFSET,y*BLOCKSIZE+YOFFSET,BLOCKSIZE,BLOCKSIZE)
				}
			})
		})
		glut_flush()  
  })

	glut_setmousefunc(fun(button:int,state:int,x:int,y:int){
		if(state==1){return}
		bm.clicked(x,y); sm.clicked(x,y)
		
		var cx:int,cy:int; /*マス目での座標*/
		if((x-XOFFSET)<0 || (y-YOFFSET)<0 || (x-XOFFSET)>(BLOCKSIZE*XSIZE) || (y-YOFFSET)>(BLOCKSIZE*YSIZE)){
			return //範囲外
		}
	
		cx=(x-XOFFSET)/BLOCKSIZE;cy=(y-YOFFSET)/BLOCKSIZE
		if(cx<0){cx=0} if(cy<0){cy=0} if(cx>XSIZE-1){cy=XSIZE-1} if(cy>YSIZE-1){cy=YSIZE-1}

		if(FIELD[cx][cy]){FIELD[cx][cy]=false}else{FIELD[cx][cy]=true}
		glut_postredisp()
	})

  if(RUNNING){glut_settimerfunc(SPEED,timerfun,1)}

  glut_mainloop()
}

data button{text:[int];callback:fun()=>void;x:int;y:int;width:int;height:int;marginx:int;marginy:int}
data buttonmanager{add:fun(button)=>void;draw:fun()=>void;clicked:fun(int,int)=>void}

fun new_buttonmanager()=>buttonmanager{
	var buttons:[button];
	return buttonmanager{
		add=fun(b:button){
			buttons=b@+buttons		
		},
		draw=fun(){
			var draw1=fun(b:button){
				glut_begin_lineloop()
				glut_color3i(0,0,0)
				glut_vertex2i(b.x,b.y);glut_vertex2i(b.x+b.width,b.y);glut_vertex2i(b.x+b.width,b.y+b.height);glut_vertex2i(b.x,b.y+b.height)
				glut_end()
				glut_charlist(b.text,b.x+b.marginx,b.y+b.marginy)				
			}
			for(0,@?buttons-1,fun(i:int){
				draw1(buttons[i])
			})
		},
		clicked=fun(x:int,y:int){
			for(0,@?buttons-1,fun(i:int){
				if(buttons[i].x<=x && buttons[i].x+buttons[i].width>=x){
					if(buttons[i].y<=y && buttons[i].y+buttons[i].height>=y){
						buttons[i].callback()
					}
				}
			})
		}
	}
}

data label{text:[int];x:int;y:int}
data labelmanager{add:fun(label)=>void;draw:fun()=>void}

fun new_labelmanager()=>labelmanager{
	var labels:[label];
	return labelmanager{
		add=fun(l:label){
			labels=l@+labels		
		},
		draw=fun(){
			var draw1=fun(l:label){
				glut_color3i(0,0,0)
				glut_charlist(l.text,l.x,l.y)				
			}
			for(0,@?labels-1,fun(i:int){
				draw1(labels[i])
			})
		}
	}
}

data spin{callback:fun(bool)=>void;x:int;y:int;marginx:int;marginy:int;}
data spinmanager{add:fun(spin)=>void;draw:fun()=>void;clicked:fun(int,int)=>void}

fun new_spinmanager()=>spinmanager{
	var spins:[spin];
	var bm=new_buttonmanager()
	return spinmanager{
		add=fun(b:spin){
			spins=b@+spins
			bm.add(button{text=['U','p'],callback=fun(){b.callback(true)},x=b.x,y=b.y,marginx=b.marginx,marginy=b.marginy,height=40,width=50})
			bm.add(button{text=['D','o','w','n'],callback=fun(){b.callback(false)},x=b.x+50,y=b.y,marginx=b.marginx,marginy=b.marginy,height=40,width=50})
		},
		draw=fun(){
			var draw1=fun(b:spin){
				bm.draw()			
			}
			for(0,@?spins-1,fun(i:int){
				draw1(spins[i])
			})
		},
		clicked=fun(x:int,y:int){
			bm.clicked(x,y)
		}
	}
}

fun for(start:int,end:int,block:fun(int)=>void){
	while(start<=end){block(start);start=start+1;}
}

fun glut_charlist(list:[int],x:int,y:int){
	for(0,@?list-1,fun(i:int){
		if(i==0){glut_char(x,y,list[i])}
		else{glut_char(-1,-1,list[i])}
	})
}
