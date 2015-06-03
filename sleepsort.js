fun main(){
	var list=[4,2,2,1,9,7,5,6,0,3,4]
	var in:channel(int)=new(channel(int))
	var i=0
	while(i<length(list)){
		async sleep_and_output(list[i],in)
		i=i+1
	}
	i=0
	while(i<length(list)){
		print(in?)
		i=i+1
	}
}

fun sleep_and_output(value:int,out:channel(int)){
	sleep(value*1000)
	out<<value
}