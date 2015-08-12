fun main(){
	var in=new(channel(int)),out=new(channel([(int,int,int)]))
	
	async calc_server(in,out)
	async calc_server(in,out)
	async calc_server(in,out)
	async calc_server(in,out)
	
	//ウィンドウを開く
	glut_openwindow("mandelbrot")
	glut_setdisplayfunc(fun(){
		print("draw\n")
		glut_clear()
		glut_begin_point()
		var y=0,n=0
		while(y<512){
			in!y
			y=y+1
		}
		while(n<512){
			var result=out?
			var t=1
			while(t<1025){
				var color=make_color(i2d(result[t][2]))
				glut_color3i(color[0],color[1],color[2])
				glut_vertex2i(result[t][0],result[t][1])
				t=t+1
			}
			n=n+1
		}
		glut_end()
		glut_flush()
	})
	glut_mainloop()
}

fun calc_server(in:channel(int),out:channel([(int,int,int)])){
	var y=in?
	var result=[(0,0,0)]
	var x=0
	while(x<1024){
		var temp=mandel_sub(x,y)
		result=result+[(x,y,temp)]
		x=x+1
	}
	print_int(y);print("\n")
	out!result
	calc_server(in,out)
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
