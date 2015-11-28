/*
	マンデルブロ集合を描画
	（描画に十数秒かかります）
*/

fun main(){
	//ウィンドウを開く
	glut_openwindow("mandelbrot")
	glut_setdisplayfunc(fun(){
		glut_clear()
		glut_begin_point()
		var y=0
		while(y<512){
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
				
				glut_color3i(d2i(temp[0]),d2i(temp[1]),d2i(temp[2]))
				glut_vertex2i(x,y)
				x=x+1
			}
			//print_int(y);print("\n");
			y=y+1
		}
		glut_end()
		glut_flush()
	})
	glut_mainloop()
}

fun mandel_sub(nowx:int,nowy:int)=>int{
	
}

fun make_color(h:double)=> (int,int,int){
	
}
