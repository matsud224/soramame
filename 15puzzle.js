fun main(){
	var board=[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,0]]
	var colorlist=[(255,255,255),(188,200,219),(211,212,155),(188,200,219),(211,212,155),(211,212,155),(188,200,219),(211,212,155),(188,200,219),(188,200,219),(211,212,155),(188,200,219),(211,212,155),(211,212,155),(188,200,219),(211,212,155)]
	var CELLSIZE=128
	var OFFSET=50
	
	randomize(board)
	
	//ウィンドウを開く
	glut_openwindow("15puzzle")
	glut_setdisplayfunc(fun(){
		glut_clear()
		if(is_ok(board)){
			glut_color3i(0,0,0)
			glut_char(200,200,'C')
			glut_char(-1,-1,'o')
			glut_char(-1,-1,'n')
			glut_char(-1,-1,'g')
			glut_char(-1,-1,'r')
			glut_char(-1,-1,'a')
			glut_char(-1,-1,'t')
			glut_char(-1,-1,'u')
			glut_char(-1,-1,'l')
			glut_char(-1,-1,'a')
			glut_char(-1,-1,'t')
			glut_char(-1,-1,'i')
			glut_char(-1,-1,'o')
			glut_char(-1,-1,'n')
			glut_char(-1,-1,'s')
			glut_char(-1,-1,'!')
			glut_flush()
			return
		}
		//枠線を描く
		glut_color3i(255,255,255)
		glut_begin_line()
		for(0,5,fun(y:int){
			glut_vertex2i(OFFSET,OFFSET+CELLSIZE*y);
			glut_vertex2i(OFFSET+CELLSIZE*4,OFFSET+CELLSIZE*y);
		})
		for(0,5,fun(x:int){
			glut_vertex2i(OFFSET+CELLSIZE*x,OFFSET);
			glut_vertex2i(OFFSET+CELLSIZE*x,OFFSET+CELLSIZE*4);
		})
		glut_end()

		//マスの描画
		
		for(0,4,fun(y:int){
			for(0,4,fun(x:int){
				glut_begin_quad()
				glut_color3i(colorlist[board[y][x]][0],colorlist[board[y][x]][1],colorlist[board[y][x]][2])
				glut_vertex2i(OFFSET+CELLSIZE*x,OFFSET+CELLSIZE*y);
				glut_vertex2i(OFFSET+CELLSIZE*(x+1),OFFSET+CELLSIZE*y);
				glut_vertex2i(OFFSET+CELLSIZE*(x+1),OFFSET+CELLSIZE*(y+1));
				glut_vertex2i(OFFSET+CELLSIZE*x,OFFSET+CELLSIZE*(y+1));
				glut_end()
				glut_color3i(66,70,111)
				if(board[y][x]!=0){glut_putnum(OFFSET+CELLSIZE*x+OFFSET/2,OFFSET+CELLSIZE*y+OFFSET/2,board[y][x])}
			})
		})
		glut_flush()
	})
	
	glut_setmousefunc(fun(button:int,state:int,x:int,y:int){
		if(is_ok(board)){return}
		var cx:int,cy:int; /*マス目での座標*/
		if(x<OFFSET || y<OFFSET || x>(OFFSET+CELLSIZE*4) /*とつぜんコメント*/ || y>(OFFSET+CELLSIZE*4)){
			return //範囲外
		}
		
		cx=(x-OFFSET)/CELLSIZE;cy=(y-OFFSET)/CELLSIZE
		if(cx<0){cx=0} if(cy<0){cy=0} if(cx>3){cy=3} if(cy>3){cy=3}
		
		if((cy,cx) >?> canmove(board)){
			var z=zeropos(board)
			board[z[0]][z[1]]=board[cy][cx]
			board[cy][cx]=0
		}
		glut_postredisp()
	})

	glut_mainloop()
}

fun is_ok(board:[[int]])=>bool{
	var init=[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,0]]
	return callcc(break,bool){
		for(0,4,fun(y:int){
			for(0,4,fun(x:int){
				if(init[y][x]!=board[y][x]){break(false)}
			})
		})
		break(true)
	}
}

fun randomize(board:[[int]]){
	for(0,30,fun(t:int){
		var r=canmove(board)
		var z=zeropos(board)
		var rn=rand() % @?r
		board[z[0]][z[1]]=board[r[rn][0]][r[rn][1]]
		board[r[rn][0]][r[rn][1]]=0
	})
}

//0-19の範囲の文字を出力
fun glut_putnum(x:int,y:int,n:int){
	if(n<10){
		glut_char(x,y,'0'+n)
	}else{
		glut_char(x,y,'1')
		glut_char(-1,-1,'0'+(n-10))
	}
}

fun for(beg:int,end:int,block:fun(int)=>void){
	var i=beg
	while(i<end){
		block(i)
		i=i+1
	}
}

fun zeropos(board:[[int]])=>(int,int){
	return callcc(break,(int,int)){
		for(0,4,fun(y:int){
			for(0,4,fun(x:int){
				if(board[y][x]==0){
					break((y,x))
				}
			})
		})
	}
}

fun canmove(board:[[int]])=>[(int,int)]{
	var result:[(int,int)]=[]
	var z=zeropos(board)
	var x0=z[1],y0=z[0];
	if(x0>=1){
		result= (y0,x0-1) @+ result
	}
	if(y0>=1){
		result= (y0-1,x0) @+ result
	}
	if(x0<=2){
		result= (y0,x0+1) @+ result
	}
	if(y0<=2){
		result= (y0+1,x0) @+ result
	}
	return result
}

//存在確認
fun >?> binary,left,50(x:(int,int),xs:[(int,int)])=>bool{
	if(@?xs==0){return false}
	if((@<xs)[0] == x[0] && (@<xs)[1] == x[1]){return true}
	return x >?> @>xs
}