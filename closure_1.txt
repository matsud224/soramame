fun main(){
  var a=make_adder(),b=make_adder(),c=make_adder()
  a();a();a();b();c();a();b();c();c();
}

fun make_adder()=> fun()=>void{
	var n=0
	return fun(){
     	print(n);print("\n")
		n=n+1
	}
}