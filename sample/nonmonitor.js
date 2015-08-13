var x:int=0;

fun main(){
	var end_notify=newchannel(int,10)
	async fun(){
		var i=1
		while(i<=100){x=x+i;i=i+1}
		end_notify ! 1
	}()
	async fun(){
		var i=1
		while(i<=100){x=x+i;i=i+1}
		end_notify ! 2
	}()

	end_notify?;end_notify?

	print("\n\nresult:");print_int(x);
}
