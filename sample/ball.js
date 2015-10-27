//ボール投げ
fun main(){
    var player=[newchannel(bool,1),
                    newchannel(bool,1),newchannel(bool,1)]
    var player1:fun()=>void = fun(){
        player[0]?
        print("Player 1 catch!\n"); sleep(3000)
        player[randitem([1,2])]!true
        player1()
    }
    var player2:fun()=>void = fun(){
        player[1]?
        print("Player 2 catch!\n"); sleep(1000)
        player[randitem([0,2])]!true
        player2()
    }
    var player3:fun()=>void = fun(){
        player[2]?
        print("Player 3 catch!\n"); sleep(2000)
        player[randitem([0,1])]!true
        player3()
    }

    async player1(); async player2(); async player3()
    player[0]!true  
    sleep(100000)
}

fun randitem(list:[int])=>int{
    return list[rand()%(@?list)]
}
