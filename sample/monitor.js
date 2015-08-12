var x:int=0;

var add_value:channel(int)=new(channel(int))
var added:channel(int)=new(channel(int))

fun main(){
	var end_notify=new(channel(int))
	async server();
	async fun(){
		var i=1
		while(i<=100){add_value ! i;added?;i=i+1}
		end_notify ! 1
	}()
	async fun(){
		var i=1
		while(i<=100){add_value ! i;added?;i=i+1}
		end_notify ! 2
	}()

	end_notify?;end_notify?

	print("\n\nresult:");print_int(x);
}

fun server(){
	var n=add_value?
	print_int(n);print("\n")
	x=x+n
	added!1
	server()
}
