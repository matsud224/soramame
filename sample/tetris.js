var NowBlock:Pair=Pair{Item1=0,Item2=0}
var Field:HashSet
var FieldSize:Size=Size{Height=0,Width=0}
var BlockPatterns:[ [ HashSet ] ]
var Offset:Point=Point{X=0,Y=0}
var BlockSize:int = 50
var Shadow:HashSet
var ShadowYOffset:int = 0
var NextBlockData:Pair=Pair{Item1=0,Item2=0}
var CurrentSpeed:int
var IsGameover:bool=false
var NEXTOFFSET:Point=Point{X=600,Y=300}
var HOLDOFFSET:Point=Point{X=600,Y=600}
var DeletedLine:int=0
var Lock:bool = true
var HoldItem:int= 3;
var AllowHold:bool =false;

/*キーコード*/
var KEY_LEFT:int = 'c';
var KEY_UP:int = 'v';
var KEY_RIGHT:int = 'm';
var KEY_DOWN:int = 'n';
var KEY_SPACE:int = ' ';
var KEY_HOLD:int = 'h';

//落ちてくるもの
var BEGIN_DATA= -1
var DIRECTION_DEF= -2
var DATA:[int]= [
				BEGIN_DATA,DIRECTION_DEF,0,0,0,1,1,0,1,1,
				BEGIN_DATA,DIRECTION_DEF,0,0,1,0,2,0,3,0,
						   DIRECTION_DEF,3,0,3,1,3,2,3,3,
						   DIRECTION_DEF,0,3,1,3,2,3,3,3,
						   DIRECTION_DEF,0,0,0,1,0,2,0,3,
				BEGIN_DATA,DIRECTION_DEF,0,0,1,0,1,1,2,1,
						   DIRECTION_DEF,0,1,1,0,1,1,0,2,
						   DIRECTION_DEF,0,0,1,0,1,1,2,1,
						   DIRECTION_DEF,0,1,1,0,1,1,0,2,
				BEGIN_DATA,DIRECTION_DEF,1,0,0,1,1,1,2,0,
						   DIRECTION_DEF,0,0,0,1,1,1,1,2,
						   DIRECTION_DEF,0,1,1,0,1,1,2,0,
						   DIRECTION_DEF,0,0,0,1,1,1,1,2,
				BEGIN_DATA,DIRECTION_DEF,0,0,0,1,1,1,2,1,
						   DIRECTION_DEF,0,0,1,0,0,1,0,2,
						   DIRECTION_DEF,0,0,1,0,2,0,2,1,
						   DIRECTION_DEF,0,2,1,0,1,1,1,2,
				BEGIN_DATA,DIRECTION_DEF,0,0,0,1,0,2,1,2,
						   DIRECTION_DEF,0,0,1,0,2,0,0,1,
						   DIRECTION_DEF,0,0,1,0,1,1,1,2,
						   DIRECTION_DEF,0,1,1,1,2,1,2,0,
				BEGIN_DATA,DIRECTION_DEF,0,1,1,0,1,1,2,1,
						   DIRECTION_DEF,0,0,0,1,0,2,1,1,
						   DIRECTION_DEF,0,0,1,0,2,0,1,1,
						   DIRECTION_DEF,0,1,1,0,1,1,1,2
				]
var BLOCKCOLORS:[(int,int,int)]=[(184,0,60),(204,86,22),(235,175,0),(0,135,71),(0,169,205),(52,87,119),(142,104,166)]

data Pair{ Item1:int; Item2:int }
data Size{ Height:int; Width:int }
data Point{ X:int; Y:int }
data YColorPair{Y:int;ColorCode:int}
data HashSet{ Add:fun(int,int,int)=>void; ForEach:fun(fun(Point,int)=>void)=>void; Contains:fun(int,int)=>bool; Remove:fun(int,int)=>void }


//テトリミノの出現確率を調整
var remain:[int] =[]
fun rand_mino()=>int{
	var intgen: fun(int)=>[int] =fun(end:int)=>[int]{
		var r:[int]=[]	
		for(0,end,fun(t:int){
			r=t@+r
		})
		return r
	}
	var randomize=fun(list:[int]){
		for(0,(@?list)*3,fun(t:int){
			var r1=rand() % @?list,r2=rand() % @?list
			var temp=list[r1]			
			list[r1]=list[r2]; list[r2]=temp
		})
	}
	if(@?remain==0){ remain=intgen((@?BlockPatterns)-1); randomize(remain) }
	var r= @<remain;remain= @>remain
	return r;
}

//HashSet(Pointを入れるの専用)
fun NewHashSet()=>HashSet{
	var contents:[(int,[YColorPair])]=[]
	
	var findindex:fun(int,[(int,[YColorPair])])=>(int,[YColorPair]) =fun(index:int,c:[(int,[YColorPair])])=>(int,[YColorPair]){
		if(@?c==0){
			return (-1,[YColorPair{Y=(-1),ColorCode=(-1)}])
		}

		if((@<c)[0]==index){return @<c}
		
		return findindex(index,@>c)
	}

	var find_y:fun(int,[YColorPair])=>bool =fun(y:int,c:[YColorPair])=>bool{
		if(@?c==0){
			return false
		}

		if((@<c).Y == y){return true}

		return find_y(y,@>c)
	}
	
	return HashSet{ 
		Add=fun(x:int,y:int,colorcode:int)=>void{
			var found=findindex(x,contents)
			if(found[0]== (-1)){contents=(x,[YColorPair{Y=y,ColorCode=colorcode}])@+contents}
			else{if(!find_y(y,found[1])){found[1]=YColorPair{Y=y,ColorCode=colorcode}@+found[1]}}
		},
		ForEach=fun(block:fun(Point,int)=>void)=>void{
			var xlen= @?contents,nx=0		
			while(nx<xlen){
				var ylen= @?(contents[nx][1]),ny=0	
				while(ny<ylen){
					block(Point{X=contents[nx][0],Y=contents[nx][1][ny].Y},contents[nx][1][ny].ColorCode)
					ny=ny+1
				}
				nx=nx+1
			}
		},
		Contains=fun(x:int,y:int)=>bool{
			var found=findindex(x,contents)
			
			if(found[0]== (-1)){return false}
			else{return find_y(y,found[1])}
		},
		Remove=fun(x:int,y:int)=>void{
			var found=findindex(x,contents)
			var remove_list=fun(value:int,c:[YColorPair])=>[YColorPair]{
				var result:[YColorPair]=[]
				var i=0,len = @?c
				while(i<len){if(value!=c[i].Y){result=c[i]@+result} i=i+1}
				return result
			}

			if(found[0]!= (-1)){found[1]=remove_list(y,found[1])}
		}
	}
}

fun for(start:int,end:int,block:fun(int)=>void){
    if(start>end){return}
    block(start)
    for(start+1,end,block)
}

fun SetShadow(){
    Shadow=NewHashSet()

    var o = Offset.Y

    callcc(break){
		while(true){
	        BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
	            if(p.Y + o > FieldSize.Height - 1){
	                break()
	            }
	            if(Field.Contains(p.X + Offset.X, p.Y + o)){
	                break()
	            }
	        })
	        o = o+1
	    }
	}
    o = o - 1
    ShadowYOffset = o

    BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
        Shadow.Add(p.X + Offset.X, p.Y + o,colorcode)
    })

}

fun DownBlock(n:int){
    callcc(ret){
		BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
		    if(p.Y + Offset.Y + 1 > FieldSize.Height - 1){
		        NextBlock(NextBlockData,true)
		        ret()
		    }
		    if(Field.Contains(p.X + Offset.X, p.Y + Offset.Y + 1)){
		        NextBlock(NextBlockData,true)
		        ret()
		    }
		})

		Offset.Y = Offset.Y+1
	}
	glut_postredisp()
	if(n==1){glut_settimerfunc(CurrentSpeed,DownBlock,1)}
}

fun Tyakuti(){
    SetShadow()
    Offset.Y = ShadowYOffset
    NextBlock(NextBlockData,true)

    glut_postredisp()
}

fun TurnBlock(){
	callcc(ret){
		var nextnum = NowBlock.Item2 + 1
		if(nextnum > @?(BlockPatterns[NowBlock.Item1]) - 1){
		    nextnum = 0
		}

		BlockPatterns[NowBlock.Item1][nextnum].ForEach(fun(p:Point,colorcode:int)=>void{
		    if(p.X + Offset.X < 0){
		        ret()
		    }
		    if(p.X + Offset.X > FieldSize.Width - 1){
		        ret()
		    }
		    if(Field.Contains(p.X + Offset.X, p.Y + Offset.Y)){
		        ret()
		    }
		})

		NowBlock.Item2 = nextnum

		glut_postredisp()
	}
}


fun NextBlock(p:Pair,nextreq:bool){
    if(nextreq){BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
        if(p.Y + Offset.Y < 0){
            Gameover()
        }
        Field.Add(p.X + Offset.X, p.Y + Offset.Y,colorcode)
    })}

    EraseLine()

    NowBlock.Item1 = p.Item1
	NowBlock.Item2 = p.Item2
	if(nextreq){
		NextBlockData.Item1 = rand_mino()
		NextBlockData.Item2 = 0
		AllowHold=true;
	}
	glut_postredisp()

    var maxx = -1000
    var maxy = -1000
    BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
        if(p.X > maxx){
            maxx = p.X
        }
        if(p.Y > maxy){
            maxy = p.Y
        }
    })

    if(@?BlockPatterns == 1){
		Offset.X= 0
	}else{
		Offset.X=abs((FieldSize.Width / 2) - (maxx / 2))
	}
    Offset.Y = (maxy * (-1))

}

fun Gameover(){
    Lock = true
    IsGameover=true
	glut_postredisp()
}

fun LeftBlock(){
	callcc(ret){
    BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
		    if(p.X + Offset.X - 1 < 0){
		        ret()
		    }
		    if(Field.Contains(p.X + Offset.X - 1, p.Y + Offset.Y)){
		        ret()
		    }
		})

		Offset.X = Offset.X-1
		glut_postredisp()
	}
}

fun RightBlock(){
	callcc(ret){
		BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
		    if(p.X + Offset.X + 1 > FieldSize.Width - 1){
		        ret()
		    }
		    if(Field.Contains(p.X + Offset.X + 1, p.Y + Offset.Y)){
		        ret()
		    }
		})

		Offset.X = Offset.X+1
		glut_postredisp()
	}
}

fun For(start:int,end:int,block:fun(int)=>void){
	while(start<=end){block(start);start=start+1;}
}

fun EraseLine(){
    var flag = true
    while( flag ){
        flag = false
        For(0,FieldSize.Height - 1,fun(i:int){
            var count = 0
            Field.ForEach(fun(p:Point,colorcode:int)=>void{
                if(p.Y == i){
                    count = count+1
                }
            })
            if(count == FieldSize.Width){
                flag = true
				DeletedLine=DeletedLine+1
				CurrentSpeed = CurrentSpeed-1
				if(CurrentSpeed < 400){
					CurrentSpeed = 400
				}
				glut_postredisp()
                For(0,FieldSize.Width - 1,fun(j:int){
                    Field.Remove(j, i)
                })
                var rmlist = NewHashSet()
                var addlist = NewHashSet()
                Field.ForEach(fun(p:Point,colorcode:int)=>void{
                    if(p.Y < i){
                        rmlist.Add(p.X,p.Y,colorcode)
                        addlist.Add(p.X, p.Y + 1,colorcode)
                    }
                })
                rmlist.ForEach(fun(p:Point,colorcode:int)=>void{
                    Field.Remove(p.X,p.Y)
                })
                addlist.ForEach(fun(p:Point,colorcode:int)=>void{
                    Field.Add(p.X,p.Y,colorcode)
                })
            }
        })
    }
}

fun Initialize(x:int, y:int){
    NextBlockData=Pair{Item1 = 0, Item2 = 0}
	CurrentSpeed=800
	IsGameover=false

    BlockPatterns=[]
    Shadow=NewHashSet()

    var cnt1 = -1
    var cnt2 = -1
	var datapos=0,datalen= @?DATA
	var empty_hash:[HashSet]=[]
    while(datapos<datalen){
		if(DATA[datapos] == BEGIN_DATA){
		    BlockPatterns=BlockPatterns + [empty_hash]
		    cnt1 = cnt1+1
		    cnt2 = (-1)
       	}else{
			if(DATA[datapos]==DIRECTION_DEF){
		        BlockPatterns[cnt1]=BlockPatterns[cnt1]+[NewHashSet()]
		        cnt2 = cnt2+1
			}else{
		        BlockPatterns[cnt1][cnt2].Add(DATA[datapos],DATA[datapos+1],cnt1)
				datapos=datapos+1
        	}
		}
		datapos=datapos+1
    }

    Field=NewHashSet()
    FieldSize=Size{Height = y,Width=x}

    NowBlock=Pair{Item1 = rand_mino(),Item2 = 0}
	NextBlockData=Pair{Item1=rand_mino(),Item2=0}
	HoldItem=rand()%(@?BlockPatterns - 1)
    var maxx = -1000
    var maxy = -1000
    BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
        if(p.X > maxx){
			maxx = p.X
        }
        if(p.Y > maxy){
            maxy = p.Y
        }
    })

    if(@?BlockPatterns == 1){
		Offset.X=0
	}else{
		Offset.X = abs((FieldSize.Width / 2) - (maxx / 2))
	}
    Offset.Y = (maxy * (-1)) + 1

}


fun KeyDown(keycode:int,x:int,y:int){
    if(Lock == true){
        return
    }
    if(keycode==KEY_LEFT){ LeftBlock(); return }
	if(keycode==KEY_RIGHT){ RightBlock(); return }
	if(keycode==KEY_DOWN){ DownBlock(0); return }
	if(keycode==KEY_SPACE){ TurnBlock(); return }
	if(keycode==KEY_UP){ Tyakuti(); return }
	if(keycode==KEY_LEFT){ LeftBlock(); return }
	if(keycode==KEY_HOLD){ Hold(); return }
}

fun Hold(){
	if(!AllowHold){return}
	var temp=HoldItem
	HoldItem=NowBlock.Item1
	NextBlock(Pair{Item1=temp},false)
	AllowHold=false
}

fun main(){
    Initialize(10, 20)
	glut_openwindow("TETRIS")

	glut_setdisplayfunc(Paint)
	glut_settimerfunc(CurrentSpeed,DownBlock,1)
	glut_setkeyboardfunc(KeyDown)
	Lock=false
	IsGameover=false
	glut_mainloop()
}

//0-10000の範囲の文字を出力
fun glut_putnum(x:int,y:int,n:int){
	var i=10000
	while(i!=1){
		if(i==10000){
			glut_char(x,y,'0'+(n%i)/(i/10))
		}else{
			glut_char(-1,-1,'0'+(n%i)/(i/10))
		}
		i=i/10
	}
}

fun Paint(){
	glut_clear()
	if(IsGameover){
		glut_color3i(0,0,0)
		glut_char(200,200,'G')
		glut_char(-1,-1,'A')
		glut_char(-1,-1,'M')
		glut_char(-1,-1,'E')
		glut_char(-1,-1,' ')
		glut_char(-1,-1,'O')
		glut_char(-1,-1,'V')
		glut_char(-1,-1,'E')
		glut_char(-1,-1,'R')
		glut_char(200,300,'L')
		glut_char(-1,-1,'I')
		glut_char(-1,-1,'N')
		glut_char(-1,-1,'E')
		glut_char(-1,-1,':')
		glut_putnum(250,300,DeletedLine)
		glut_flush()
		return
	}

	glut_color3i(0,0,0)
		glut_char(600,70,'L')
		glut_char(-1,-1,'I')
		glut_char(-1,-1,'N')
		glut_char(-1,-1,'E')
		glut_char(-1,-1,':')
	glut_putnum(600,100,DeletedLine)

	
	glut_color3i(10,10,10)
    FillRectangle(0, 0, BlockSize * FieldSize.Width, BlockSize * FieldSize.Height)

    Field.ForEach(fun(p:Point,colorcode:int)=>void{
		var c=GetColor(colorcode)
        DrawBlock(p.X * BlockSize, p.Y * BlockSize, BlockSize, BlockSize,c)
    })

    SetShadow()

    Shadow.ForEach(fun(p:Point,colorcode:int)=>void{
		glut_color3i(100,100,100)
        FillRectangle(p.X * BlockSize, p.Y * BlockSize, BlockSize, BlockSize)
    })

    BlockPatterns[NowBlock.Item1][NowBlock.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
        if(p.X + Offset.X >= 0 && p.X + Offset.X <= FieldSize.Width - 1 && p.Y + Offset.Y >= 0 && p.Y + Offset.Y <= FieldSize.Height - 1){
			var c=GetColor(colorcode)
            DrawBlock((p.X + Offset.X) * BlockSize, (p.Y + Offset.Y) * BlockSize, BlockSize, BlockSize,c)
        }
    })
	

	glut_color3i(0,0,0)
		glut_char(600,270,'N')
		glut_char(-1,-1,'E')
		glut_char(-1,-1,'X')
		glut_char(-1,-1,'T')
		glut_char(-1,-1,':')
    BlockPatterns[NextBlockData.Item1][NextBlockData.Item2].ForEach(fun(p:Point,colorcode:int)=>void{
		var c=GetColor(colorcode)
        DrawBlock(NEXTOFFSET.X+(p.X) * BlockSize, NEXTOFFSET.Y+(p.Y) * BlockSize, BlockSize, BlockSize,c)
    })

	glut_color3i(0,0,0)
		glut_char(600,570,'H')
		glut_char(-1,-1,'O')
		glut_char(-1,-1,'L')
		glut_char(-1,-1,'D')
		glut_char(-1,-1,':')
    BlockPatterns[HoldItem][0].ForEach(fun(p:Point,colorcode:int)=>void{
		var c=GetColor(colorcode)
        DrawBlock(HOLDOFFSET.X+(p.X) * BlockSize, HOLDOFFSET.Y+(p.Y) * BlockSize, BlockSize, BlockSize,c)
    })


	glut_flush()
}

fun DrawRectangle(x:int,y:int,w:int,h:int){
	glut_begin_lineloop()
	glut_vertex2i(x,y);
	glut_vertex2i(x,y+h);
	glut_vertex2i(x+w,y+h);
	glut_vertex2i(x+w,y);
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

fun glut_color(c:(int,int,int)){glut_color3i(c[0],c[1],c[2])}

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

fun tocolor(a:int)=>int{if(a<0){return 0} if(a>255){return 255} return a}

fun brightness(inc:int,color:(int,int,int))=>(int,int,int){
	return (tocolor(color[0]+inc),tocolor(color[1]+inc),tocolor(color[2]+inc))
}

fun GetColor(n:int)=>(int,int,int){
	return BLOCKCOLORS[n% @?BLOCKCOLORS]
}

