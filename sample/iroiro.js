fun main(){
	//クイックソート（昇順）
	print_intlist(-->[2,5,2,1,6,8,4,2,-65,0,875,2,34,21,-4,32,13,5]);
	print("\n")
	//クイックソート（降順）
	print_intlist(<--[2,5,2,1,6,8,4,2,-65,0,875,2,34,21,-4,32,13,5]);

	//べき乗
	print("\n\n");
	print_int(pow(2,5));
	print("\n")
	print_int(2^5)
	print("\n")
	print_int(2^3^2)

	//filter関数
	print("\n\nfilter:");
	print_intlist(filter(fun(x:int){return (x%2==0)},[1,2,3,4,5,6,7,8,9]));
	print("\nfilter:");
	var odd=fun(x:int){return (x%2==1)}
	print_intlist(filter(odd,[1,2,3,4,5,6,7,8,9]));

	//map関数
	print("\n\nmap:");
	print_intlist(map(fun(x:int){return x^2},[1,2,3,4,5,6,7,8,9]));

	//関数合成
	print("\n\ncomposite:");
	var f=fun(x:int){return x*2}
	var g=fun(x:int){return x*x}
	print_intlist(map(f$g,[1,2,3,4,5,6,7,8,9]));
}

fun print_intlist(lst:[int]){
	if(@?lst==0){return}
	print_int(@<lst)
	print(",")
	print_intlist(@>lst)
}

//クイックソート
fun qsort(lst:[int])=>[int]{
	if(@?lst==0){return []}
	var x= @<lst,xs= @>lst
	var i=0
	var smaller:[int]=[],larger:[int]=[]
	
	while(i< @?xs){
		if(xs[i] <= x){
			smaller= xs[i] @+ smaller
		}else{
			larger= xs[i] @+ larger
		}
		i=i+1
	}
	return qsort(smaller) + [x] + qsort(larger)
}

fun reverse(lst:[int])=>[int]{
	if(@?lst==0){return []}
	return reverse(@>lst) + [@<lst]
}

//昇順にソートする単項演算子
fun --> unary,right,70 (target:[int])=>[int]{
	return qsort(target)
}

//降順にソートする単項演算子
fun <-- unary,right,70 (target:[int])=>[int]{
	return reverse(qsort(target))
}

//べき乗演算子
fun ^ binary,right,50(x:int,y:int)=>int{
	return pow(x,y)
}

fun filter(pred:fun(int)=>bool,lst:[int])=>[int]{
	if(@?lst==0){return []}
	if(!pred(@<lst)){return filter(pred,@>lst)}
	else{return @<lst @+ filter(pred,@>lst)}
}

fun map(f:fun(int)=>int,lst:[int])=>[int]{
	if(@?lst==0){return []}
	return f(@<lst) @+ map(f,@>lst)
}

fun concat(xss:[[int]])=>[int]{
	var xs:[int]=[]
	var i=0
	while(i< @?xss){
		xs=xs+xss[i]
		i=i+1
	}
	return xs
}

//関数合成演算子
fun $ binary,right,60(f:fun(int)=>int,g:fun(int)=>int)=>fun(int)=>int{
	return fun(x:int){
		return f(g(x))
	}
}
