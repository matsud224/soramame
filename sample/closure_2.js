fun main(){
	var c=0
	var f=fun(){
		print_int(c);print("\n")
	}
	
	f();c=777;f();c=23;f();
}
