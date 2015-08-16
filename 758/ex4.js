//継続で脱出
fun main(){
	var x=callcc(c,int){
		return product([1,2,3,0,6,7],c)
	}
	
	print_int(x)
	print("\n")
}

fun product(lst:[int],cont:continuation(int))=>int
{
	if(@?lst==0){
		return 1
	}
	if(@<lst==0){
		cont(0)
	}else{
		return @<lst * product(@>lst,cont)
	}
}
