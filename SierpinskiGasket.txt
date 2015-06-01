var maxLevel:int = 6


fun main() {
	glut_openwindow("SierpinskiGasket")
	glut_setdisplayfunc(fun(){
		glut_clear()
	
		glut_color3i(163,73,164)
		glut_begin_line()
    	drawSierpinskiGasket(319, 40, 30, 430, 609, 430, 0)
		glut_end()
		glut_flush()  
	})

	glut_mainloop()
}

fun drawLine(x1:int,y1:int,x2:int,y2:int){
	glut_vertex2i(x1,y1)
	glut_vertex2i(x2,y2)
}

fun drawSierpinskiGasket(x1:int,y1:int,x2:int,y2:int,x3:int,y3:int,level:int) {
    if (level == maxLevel) {
        drawLine(x1, y1, x2, y2)
        drawLine(x2, y2, x3, y3)
        drawLine(x3, y3, x1, y1)

    } else {
        var xx1 = (x1 + x2) / 2
        var yy1 = (y1 + y2) / 2
        var xx2 = (x2 + x3) / 2
        var yy2 = (y2 + y3) / 2
        var xx3 = (x3 + x1) / 2
        var yy3 = (y3 + y1) / 2

        level=level+1

        drawSierpinskiGasket(x1, y1, xx1, yy1, xx3, yy3, level)
        drawSierpinskiGasket(x2, y2, xx1, yy1, xx2, yy2, level)
        drawSierpinskiGasket(x3, y3, xx3, yy3, xx2, yy2, level)
    }
}