//クロージャのテスト
fun make_adder()=> fun()=>int{
	var n=0
	return fun(){
		n=n+1; return n;
	}
}

fun main(){
	var a=make_adder()
	var b=make_adder()
	
	print_int(a())
	print_int(b())

	print_int(a())
	print_int(b())
	
	print("\n")
}
