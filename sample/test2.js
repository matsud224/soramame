//継続で脱出
fun main(){
	var x=callcc(c,int){
		return product([1,2,3],c)
	}
	
	print_int(x)
}

fun product(lst:[int],cont:continuation(int))=>int
{
	if(@?lst==0){
		return 1
	}
	if(@<lst==0){
		print("zero")
		cont(0)
	}else{
		return @<lst*product(@>lst,cont)
	}
}
