fun main(){
	var out=newchannel( (int,int,vector(int),vector(int),vector(int) ) ,4)
	
	async calc_server(0,159,out)
	async calc_server(160,255,out)
	async calc_server(256,351,out)
	async calc_server(352,511,out)
	
	//async calc_server(0,255,out)
	//async calc_server(256,511,out)
	//ウィンドウを開く
	
	var f=fun(){
		var n=0
		while(n<4){
			var result=out?
			var y=result[1],y_start=result[0]
			while(y>=y_start){
				var x=1023
				while(x>=0){
					var index=1024*(y-y_start)+x
					x=x-1
				}
				y=y-1
			}
			n=n+1
		}
	}
	f()
}

fun calc_server(y_start:int,y_end:int,out:channel( (int,int,vector(int),vector(int),vector(int) ) ) ){
	var y=y_start
	var result_r=newvector( int, (y_end-y_start+1)*1024)
	var result_g=newvector( int, (y_end-y_start+1)*1024)
	var result_b=newvector( int, (y_end-y_start+1)*1024)
	while(y<=y_end){
		var x=0
		while(x<1024){
			var zx=0.0,zy=0.0,nextx=0.0,nexty=0.0,a=(i2d(x)-512.0)/200.0,b=(i2d(y)-256.0)/200.0
			var h= i2d(callcc(break,int){
				var i=0
				while(i<60){
					if(zx*zx+zy*zy>4.0){
						break(i)
					}
					nextx=zx*zx-zy*zy+a
					nexty=2.0*zx*zy+b
					zx=nextx;zy=nexty
					i=i+1
				}
				break(0)
			})

			var s=95.0,v=210.0,max=v,min=max-((s/255.0)*max)
			h=h*6.0
			var temp:(double,double,double)=callcc(break,(double,double,double)){
				if(h<60.0){
					break((max,(h/60.0)*(max-min)+min,min))
				}
				if(h<120.0){
					break((((120.0-h)/60.0)*(max-min)+min,max,min))
				}
				if(h<180.0){
					break((min,max,((h-120.0)/60.0)*(max-min)+min))
				}
				if(h<240.0){
					break((min,((240.0-h)/60.0)*(max-min)+min,max))
				}
				if(h<300.0){
					break((((h-240.0)/60.0)*(max-min)+min,min,max))
				}
				if(h<=360.0){
					break((max,min,((360.0-h)/60.0)*(max-min)+min))
				}
			}
			var index=(y-y_start)*1024+x
			result_r[index]=d2i(temp[0])
			result_g[index]=d2i(temp[1])
			result_b[index]=d2i(temp[2])
			x=x+1
		}
		y=y+1;//print_int(y);print("\n")
	}
	print("Finished!\n")
	out!(y_start,y_end,result_r,result_g,result_b)
}

fun mandel_sub(nowx:int,nowy:int)=>int{
	var zx=0.0,zy=0.0,nextx=0.0,nexty=0.0,a=(i2d(nowx)-512.0)/200.0,b=(i2d(nowy)-256.0)/200.0
	
	return callcc(break,int){
		var i=0
		while(i<60){
			if(zx*zx+zy*zy>4.0){
				break(i)
			}
			nextx=zx*zx-zy*zy+a
			nexty=2.0*zx*zy+b
			zx=nextx;zy=nexty
			i=i+1
		}
		break(0)
	}
}

fun make_color(h:double)=> (int,int,int){
	var s=95.0,v=210.0,max=v,min=max-((s/255.0)*max)
	h=h*6.0
	var temp:(double,double,double)=callcc(break,(double,double,double)){
		if(h<60.0){
			break((max,(h/60.0)*(max-min)+min,min))
		}
		if(h<120.0){
			break((((120.0-h)/60.0)*(max-min)+min,max,min))
		}
		if(h<180.0){
			break((min,max,((h-120.0)/60.0)*(max-min)+min))
		}
		if(h<240.0){
			break((min,((240.0-h)/60.0)*(max-min)+min,max))
		}
		if(h<300.0){
			break((((h-240.0)/60.0)*(max-min)+min,min,max))
		}
		if(h<=360.0){
			break((max,min,((360.0-h)/60.0)*(max-min)+min))
		}
	}
	return (d2i(temp[0]),d2i(temp[1]),d2i(temp[2]))
}
