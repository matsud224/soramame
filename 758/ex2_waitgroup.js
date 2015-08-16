//並行実行・チャンネル通信のテスト

#include <waitgroup.js>

fun main(){
	var wg=waitgroup_new()
	
	waitgroup_add(wg);
	async fun(){
		sleep(1000)
		print("hello\n")
		waitgroup_done(wg)
	}()

	waitgroup_add(wg)
	async fun(){
		sleep(2000)
		print("world!\n")
		waitgroup_done(wg)
	}()

	waitgroup_wait(wg)
}
