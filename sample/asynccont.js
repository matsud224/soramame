fun main(){
	var a:continuation(int)
	var x=1 + callcc(c,int){
		a=c
		return 3
	}
	print_int(x)
	if(x==4){
		async f1(a)
		sleep(1000)
	}
}

fun f1(c:continuation(int)){
	c(5)
}

fun mypow(x:int,y:int)=>int{
	return pow(x,y)
}
