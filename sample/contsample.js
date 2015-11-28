/*
	productは、リストの要素の積を求める関数
	0を見つけると継続で直ちに脱出する
*/

fun main(){
	var x=callcc(c,int){
		return product([1,2,0,3],c)
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
