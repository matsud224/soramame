//並行実行・チャンネル通信のテスト
//1秒後にhello, もう1秒後にworldが表示される
//最後の c? によってスレッドと同期している

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
