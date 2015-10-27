fun main(){
	print_int(2^3);print("\n")
	var f= fun(x:int)=>int{ return x*x }
	var g= fun(x:int)=>int{ return x+2 }
	print_int( (f$g)(3) )
}

/*fun ^ binary,right,50(x:int,y:int)=>int{
	return pow(x,y)
}
*/
fun $ binary,right,60(f:fun(int)=>int,g:fun(int)=>int)=>fun(int)=>int{
	return fun(x:int){ return f(g(x)) }
}
