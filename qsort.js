fun main(){
	var result=qsort([2,5,2,1,6,8,4,2,-65,0,875,2,34,21,-4,32,13,5])
	var i=0
	while(i< @?result){
		print_int(result[i]);print(", ")
		i=i+1
	}
}

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