//末尾呼び出し（ f(),g() ）は末尾呼び出し最適化によりgoto相当になる
fun main(){
	f()
}

fun f(){
	print("f")
	g()
}

fun g(){
	print("g")
	f()
}
