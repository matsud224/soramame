/* リサージュ曲線 */
fun main(){
    glut_openwindow("Lissajous")
    glut_setdisplayfunc(fun(){
        glut_clear()
        glut_begin_point()
        var a=4.0,b=3.0,t=0.0
        while(t<1000.0){
            glut_color3i(255,0,0)
            glut_vertex2i(d2i(sin(a*t)*50.0)+100,
                        d2i(cos(b*t)*50.0)+100)
            t=t+1.0
        }
        glut_end()
        glut_flush()
  })
  glut_mainloop()
}
