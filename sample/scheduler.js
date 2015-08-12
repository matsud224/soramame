var scheduler:continuation(int)
var process1:continuation(int)
var process2:continuation(int)

fun main(){
	var a=callcc(sc,int){scheduler=sc;return 0}

	if(a==0){
		f()
		return
	}
	if(a==1){
		g()
		return
	}
	if(a==2){
		process1(-1)
		return
	}
	if(a==11){
		process2(-1)
		return
	}
	if(a==21){
		process1(-1)
		return
	}
}

fun f(){
	var a=callcc(p1,int){
		process1=p1
		return 0
	}
	if(a!=0){
		print("Hello\n")
		scheduler(11)
	}
	scheduler(1)
}

fun g(){
	var a=callcc(p2,int){
		process2=p2
		return 0
	}
	if(a!=0){
		print("World\n")
		scheduler(21)
	}
	scheduler(2)
}