fun main(){
	var record:[(string,int)]=[]
	record=income(record,"“¹‚ÅE‚Á‚½",\100)
	record=income(record,"‹ó`‚ÅE‚Á‚½",$2)
	record=expenses(record,"ƒAƒCƒXƒNƒŠ[ƒ€",$1)
	record=expenses(record,"•å‹à",\10)
	show(record)
}

fun expenses(o:[(string,int)],desc:string,x:int)=>[(string,int)]{
	return o + [(desc,-x)];
}

fun income(o:[(string,int)],desc:string,x:int)=>[(string,int)]{
	return o + [(desc,x)];
}

fun show(o:[(string,int)]){
	var i=0,total=0
	while(i< @?o){
		print(o[i][0] + " : \\");print_int(o[i][1]);print("\n")
		total=total+o[i][1]
		i=i+1
	}
	print("------------\n")
	print("\\");print_int(total);print("\n")
}

fun \ unary,right,90(x:int)=>int{
	return x
}

fun $ unary,right,90(x:int)=>int{
	return x*125
}