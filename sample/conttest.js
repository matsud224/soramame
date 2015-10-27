fun main(){
    var cont:continuation(void)
    print("do "); print("re "); print("mi ")
    callcc(c){ /*cに継続が代入されている*/ cont=c  }
    print("fa "); print("so "); print("ra ")
    cont()
}
