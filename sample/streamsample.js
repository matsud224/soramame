//ストリーム
fun main(){
    var c1=newchannel(int,1)
    var c2=newchannel(int,1)
    var c3=newchannel(int,1)
    async intgen(c1,1)
    async map(fun(x:int){return x*x},c1,c2)
    async filter(fun(x:int){return x%17==0},c2,c3)
    
    print_int(c3?); print("\n")
    print_int(c3?); print("\n")
    print_int(c3?); print("\n")
}

fun intgen(chan:channel(int),start:int){
    chan ! start
    intgen(chan,start+1)
}

fun filter(f:fun(int)=>bool,in:channel(int),out:channel(int)){
    var value=in?	
    if(f(value)){ out!value }
    filter(f,in,out)
}

fun map(f:fun(int)=>int,in:channel(int),out:channel(int)){
    out ! f(in?)
    map(f,in,out)
}
