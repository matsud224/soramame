//並行実行・チャンネル通信のテスト
fun main(){
	var c=newchannel(int,2)
	
	async fun(){
		sleep(1000)
		print("hello\n") ; c ! 1;
	}()

	async fun(){
		sleep(2000)
		print("world!\n") ; c ! 2;
	}()

	c?;
	c?;
}
