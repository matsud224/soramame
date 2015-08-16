data WaitGroup{
	threads:int
	chan:channel(bool)
}

fun waitgroup_new()=>WaitGroup{
	var wg=WaitGroup{threads=0,chan=newchannel(bool,10)}
	return wg
}

fun waitgroup_add(wg:WaitGroup){
	wg.threads=wg.threads+1
}

fun waitgroup_done(wg:WaitGroup){
	wg.chan ! true
}

fun waitgroup_wait(wg:WaitGroup){
	var i=wg.threads
	while(i>0){
		wg.chan?; i=i-1
	}
}
