#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(){
    int pip1[2] = {};
    int pip2[2] = {};

    //注册一个管道，端口pip1[0]表示读入，pip1[1]表示写入
    if(pipe(pip1) < 0){
        //此时代表分配管道失败
        printf("Error 201 : Failed to allocate pipeline!");
        exit(-201);
    }
        //注册一个管道，端口pip2[0]表示读入，pip2[1]表示写入
    if(pipe(pip2) < 0){
        //此时代表分配管道失败
        printf("Error 202 : Failed to allocate pipeline!");
        exit(-202);
    }
    
    int forkPid = fork();

    if(forkPid == 0){
        //此时代表进入了子进程
        char buf[1];
        read(pip1[0],buf,1);
        close(pip1[0]);
        printf("%d: received ping\n",getpid());
        write(pip2[1],buf,1);
        close(pip2[1]);

    }
    else{
        //此时代表进入了父进程
        char buf[1] = {'a'};
        //向管道内写入数据
        write(pip1[1],buf,1);
        close(pip1[1]);
        read(pip2[0],buf,1);
        close(pip2[0]);
        printf("%d: received pong\n",getpid());
    }


    exit(0);
}
