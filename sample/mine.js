//(int,int)は、開閉/旗状態と地雷有無
var board:vector(vector((int,bool)))
var STATUS_CLOSE:int=0,STATUS_OPEN:int=1,STATUS_FLAG:int=2
var XSIZE:int=30,YSIZE:int=16,MINES:int=99
var BLOCKSIZE:int=50
var FIRST:bool=true
var GAMEOVER:bool=false
var GAMECLEAR:bool=false
var OPENED:int
var FLAG:int

fun makeboard()=>vector(vector((int,bool))){
	var board=newvector(vector((int,bool)),XSIZE)
	for(0,XSIZE-1,fun(x:int){
		board[x]=newvector((int,bool),YSIZE)
		for(0,YSIZE-1,fun(y:int){
			board[x][y]=(STATUS_CLOSE,false)
		})
	})
	return board
}

fun check_gameclear(){
	if(FLAG!=MINES && OPENED!=XSIZE*YSIZE-MINES){return}
	for(0,XSIZE-1,fun(x:int){
		for(0,YSIZE-1,fun(y:int){
			if((!(board[x][y][1] && board[x][y][0]==STATUS_FLAG)) || (!(!board[x][y][1] && board[x][y][0]==STATUS_OPEN))){
				return;
			}
		})
	})
	GAMECLEAR=true;
	for(0,XSIZE-1,fun(x:int){
		for(0,YSIZE-1,fun(y:int){
			if(board[x][y][1]){
				board[x][y][0]=STATUS_FLAG
			}
		})
	})
}


fun setmines(mx:int,my:int){
	OPENED=0
	FLAG=0
	for(0,MINES-1,fun(i:int){
		var retry:continuation(void)
		callcc(c){retry=c}
		var x=rand()%XSIZE,y=rand()%YSIZE
		if(((x>=mx-1 && x<=mx+1) && (y>=my-1 && y<=my+1))){
			retry()
		}
		if(board[x][y][1]){
			retry()
		}else{
			board[x][y][1]=true
		}
	})
	for(0,XSIZE-1,fun(x:int){
		for(0,YSIZE-1,fun(y:int){
			if(((x>=mx-1 && x<=mx+1) && (y>=my-1 && y<=my+1))){
				open_with_neighbor(x,y)
			}
		})
	})
}

fun for(start:int,end:int,block:fun(int)=>void){
	while(start<=end){block(start);start=start+1;}
}

fun glut_put1(x:int,y:int,n:int){
	glut_char(x,y,n)
	glut_char(x+1,y+1,n)
}


fun open(cx:int,cy:int){
	if(board[cx][cy][0]!=STATUS_OPEN){
		OPENED=OPENED+1
		board[cx][cy][0]=STATUS_OPEN
	}
	if(board[cx][cy][1]){
		//地雷
		gameover()
	}
	check_gameclear()
}


fun retexist(x:int,y:int)=>int{
	if(x<0||x>=XSIZE||y<0||y>=YSIZE){return 0}
	else{if(board[x][y][1]){return 1}else{return 0}}
}

fun open_with_neighbor(cx:int,cy:int){
	
	var retflag=fun(x:int,y:int)=>int{
					if(x<0||x>=XSIZE||y<0||y>=YSIZE){return 0}
					else{if(board[x][y][0]==STATUS_FLAG){return 1}else{return 0}}
				}
	//周囲もオープンできるならオープン
	if(cx<0||cx>=XSIZE||cy<0||cy>=YSIZE){return}
	open(cx,cy)
	if(GAMEOVER || GAMECLEAR){return}
	var count=retexist(cx-1,cy-1)+retexist(cx,cy-1)+retexist(cx+1,cy-1)+retexist(cx-1,cy)+retexist(cx+1,cy)+retexist(cx-1,cy+1)+retexist(cx,cy+1)+retexist(cx+1,cy+1)
	var fcount=retflag(cx-1,cy-1)+retflag(cx,cy-1)+retflag(cx+1,cy-1)+retflag(cx-1,cy)+retflag(cx+1,cy)+retflag(cx-1,cy+1)+retflag(cx,cy+1)+retflag(cx+1,cy+1)
	if(count==fcount){
		nonflag_safe_open(cx-1,cy-1);nonflag_safe_open(cx,cy-1);nonflag_safe_open(cx+1,cy-1);nonflag_safe_open(cx-1,cy);
		nonflag_safe_open(cx+1,cy);nonflag_safe_open(cx-1,cy+1);nonflag_safe_open(cx,cy+1);nonflag_safe_open(cx+1,cy+1);
	}
}

fun nonflag_safe_open(cx:int,cy:int){
	if(cx<0||cx>=XSIZE||cy<0||cy>=YSIZE){return}
	if(board[cx][cy][0]==STATUS_CLOSE){
		open_with_neighbor(cx,cy)
	}
}

fun gameover(){
	for(0,XSIZE-1,fun(x:int){
		for(0,YSIZE-1,fun(y:int){
			if(board[x][y][1]){
				board[x][y][0]=STATUS_OPEN
			}
		})
	})
	GAMEOVER=true;
}

fun main(){
	board=makeboard()

	
	glut_openwindow("MINESWEEPER")
	glut_setdisplayfunc(fun(){
		glut_clear()
		for(0,XSIZE-1,fun(x:int){
			for(0,YSIZE-1,fun(y:int){
				if(board[x][y][0]==STATUS_CLOSE){
					DrawBlock(BLOCKSIZE*x,BLOCKSIZE*y,BLOCKSIZE,BLOCKSIZE,(198,195,198))
				}
				if(board[x][y][0]==STATUS_FLAG){
					DrawBlock(BLOCKSIZE*x,BLOCKSIZE*y,BLOCKSIZE,BLOCKSIZE,(198,195,198))
					glut_color3i(255,0,0)
					glut_put1(BLOCKSIZE*x+20,BLOCKSIZE*y+30,'F')
				}
				if(board[x][y][0]==STATUS_OPEN){
					glut_color3i(198,195,198)
					FillRectangle(BLOCKSIZE*x,BLOCKSIZE*y,BLOCKSIZE,BLOCKSIZE)
					glut_color3i(132,130,132)
					DrawRectangle(BLOCKSIZE*x,BLOCKSIZE*y,BLOCKSIZE,BLOCKSIZE)
					glut_color3i(0,0,0)
					if(board[x][y][1]){glut_put1(BLOCKSIZE*x+20,BLOCKSIZE*y+30,'*');return}
					
					var count=retexist(x-1,y-1)+retexist(x,y-1)+retexist(x+1,y-1)+retexist(x-1,y)+retexist(x+1,y)+retexist(x-1,y+1)+retexist(x,y+1)+retexist(x+1,y+1)
					glut_color3i(0,0,0)
					if(count!=0){
						var color=[(0,0,255),(0,128,0),(255,0,0),(0,0,128),(128,0,0),(0,128,128),(128,128,0),(128,0,128)]
						glut_color(color[count])
						glut_put1(BLOCKSIZE*x+20,BLOCKSIZE*y+30,'0'+count)
					}
					
				}
			})
		})

		glut_end()
		glut_flush()  
  })

	glut_setmousefunc(fun(button:int,state:int,x:int,y:int){
			if(GAMEOVER || GAMECLEAR){return}
			var cx:int,cy:int; /*マス目での座標*/
			if(x<0 || y<0 || x>(BLOCKSIZE*XSIZE) || y>(BLOCKSIZE*YSIZE)){
				return //範囲外
			}
		
			cx=x/BLOCKSIZE;cy=y/BLOCKSIZE
			if(cx<0){cx=0} if(cy<0){cy=0} if(cx>XSIZE-1){cy=XSIZE-1} if(cy>YSIZE-1){cy=YSIZE-1}
			
			if(FIRST){
				setmines(cx,cy)
				FIRST=false
			}
			
			if(state==0 && button==0){
				if(board[cx][cy][0]!=STATUS_FLAG){
					open_with_neighbor(cx,cy)
				}
			}
			if(state==0 && button==2){
				if(board[cx][cy][0]==STATUS_FLAG){
					FLAG=FLAG+1
					board[cx][cy][0]=STATUS_CLOSE
				}else{
					if(board[cx][cy][0]==STATUS_CLOSE){
						board[cx][cy][0]=STATUS_FLAG
					}
				}
			}

			glut_postredisp()
		})
  glut_mainloop()
}



fun DrawBlock(x:int,y:int,w:int,h:int,c:(int,int,int)){
	glut_color(c)
	FillRectangle(x,y,w,h)
	var bright1=brightness(50,c),bright2=brightness(25,c)
	var dark1=brightness(-25,c),dark2=brightness(-50,c)
	glut_color(bright1)
	glut_begin_quad()
	glut_vertex2i(x,y); glut_vertex2i(x+w/8,y+h/8);
	glut_vertex2i(x+w/8,y+h-h/8); glut_vertex2i(x,y+h);
	glut_end()
	glut_color(bright2)
	glut_begin_quad()
	glut_vertex2i(x,y); glut_vertex2i(x+w,y);
	glut_vertex2i(x+w-w/8,y+h/8); glut_vertex2i(x+w/8,y+h/8);
	glut_end()
	glut_color(dark1)
	glut_begin_quad()
	glut_vertex2i(x+w/8,y+h-h/8); glut_vertex2i(x+w-w/8,y+h-h/8);
	glut_vertex2i(x+w,y+h); glut_vertex2i(x,y+h);
	glut_end()
	glut_color(dark2)
	glut_begin_quad()
	glut_vertex2i(x+w-w/8,y+h/8); glut_vertex2i(x+w-w/8,y+h-h/8);
	glut_vertex2i(x+w,y+h); glut_vertex2i(x+w,y);
	glut_end()
}

fun FillRectangle(x:int,y:int,w:int,h:int){
	glut_begin_quad()
	glut_vertex2i(x,y);
	glut_vertex2i(x,y+h);
	glut_vertex2i(x+w,y+h);
	glut_vertex2i(x+w,y);
	glut_end()
}

fun DrawRectangle(x:int,y:int,w:int,h:int){
	glut_begin_lineloop()
	glut_vertex2i(x,y);
	glut_vertex2i(x,y+h);
	glut_vertex2i(x+w,y+h);
	glut_vertex2i(x+w,y);
	glut_end()
}

fun glut_color(c:(int,int,int)){glut_color3i(c[0],c[1],c[2])}
fun tocolor(a:int)=>int{if(a<0){return 0} if(a>255){return 255} return a}

fun brightness(inc:int,color:(int,int,int))=>(int,int,int){
	return (tocolor(color[0]+inc),tocolor(color[1]+inc),tocolor(color[2]+inc))
}

