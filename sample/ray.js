//Ray Tracing Project(http://www.not-enough.org/abe/manual/ray-project/)

data VECTOR{
	x:double
	y:double
	z:double
}

fun main()
{
	glut_openwindow("ray tracing")
	glut_setdisplayfunc(fun(){
		nowx=0;nowy=0
		glut_clear()
		glut_begin_point()
		scan()
		glut_end()
		glut_flush()
	})
	glut_mainloop()
}

var nowx:int=0,nowy:int=0
fun update(){
	nowx=nowx+1
	if(nowx==200){
		nowx=0;nowy=nowy+1;print_int(nowy);print("\n")
	}
}

fun scan()
{
	var radius:double,b:double,c:double,d:double,t:double
	var det:double
	var from=VECTOR{},to=VECTOR{},vector=VECTOR{},point=VECTOR{}
	
	point.x = 0.0
	point.y = 0.0
	point.z = 0.0
	radius = 0.5

	from.z = 3.0
	to.x = 0.0
	to.y = 0.0
	to.z = (-1.0)

	from.y = (-1.0)
	while(from.y < 1.0)
	{
		from.x = (-1.0)
		while(from.x < 1.0)
		{
			callcc(continue){
				vminus(from,point,vector)
				b = innerproduct(to,vector)
				c = innerproduct(vector,vector) - radius*radius
				d = b * b - c
				if(d < 0.0)
				{
					glut_color3i(100,100,100)
					glut_vertex2i(nowx,nowy)
					update()
					continue()
				}
				det = sqrt(d)
				t = (-b) + det
				if(t < 0.0)
				{
					glut_color3i(100,100,100)
					glut_vertex2i(nowx,nowy)
					update()
					continue()
				}
				sphere_color(point, from, t)
			}
			from.x = from.x+ 2.0/200.0
		}

		from.y = from.y + 2.0/200.0
	}
}


fun sphere_color(point:VECTOR, to:VECTOR, t:double)
{
	var col:int
	var d:double
	var v1=VECTOR{}, v2=VECTOR{}
	var l = VECTOR{x=0.5773, y=(-0.5773), z=0.5773}

	vmulti(t, to, v1)
	vminus(v1, point, v2)
	vnormalize(v2)
	d = innerproduct(l, v2)
	col = d2i(255.0*d) + 50
	if(col > 255){
		col = 255
	}
	if(col < 0){
		col = 0
	}

	glut_color3i(col,0,0)
	glut_vertex2i(nowx,nowy)
	update()
}

fun vmulti(t:double, a:VECTOR, b:VECTOR)
{
	b.x = t * a.x
	b.y = t * a.y
	b.z = t * a.z
}

fun vminus(a:VECTOR,b:VECTOR,c:VECTOR)
{
	c.x = a.x - b.x
	c.y = a.y - b.y
	c.z = a.z - b.z
}

fun vnormalize(a:VECTOR)
{
	var d=sqrt(a.x*a.x + a.y*a.y + a.z*a.z)
	a.x = a.x/d
	a.y = a.y/d
	a.z = a.z/d
}

fun innerproduct(a:VECTOR, b:VECTOR)=>double
{
	return a.x * b.x + a.y * b.y + a.z * b.z
}