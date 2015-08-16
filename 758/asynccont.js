/* asyncにより別スレッドで動いている関数から
　　　main関数で作った継続を評価 */
fun main(){
	var cont:continuation(int)
	var x= 1 + callcc(c,int){
		cont=c
		return 3
	}
	print_int(x)
	if(x==4){
		async f1(cont)
		sleep(1000)
		print("\n")
	}
}

fun f1(c:continuation(int)){
	c(5)
}
