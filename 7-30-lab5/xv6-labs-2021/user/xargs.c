#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define STD_INPUT 0

int main(int argc ,char ** argv){
    //如果此时参数个数小于1，则不进行操作，即此时没指定后面要进行什么指令
    if(argc<2){
        printf("Err :We need command!\n");
        exit(-1);
    }
    //  char* args[MAXARG+1];
    // int index = 0;
    // for (int i=1; i<argc; ++i) {
    //     args[index++] = argv[i];
    // }

    char * arguments[MAXARG +1];
    int index = 0;
    for (int i=1;i<argc;i++){
        arguments[index] = argv[i];
        index++;
    }

    char buf[1024];//此时对应缓冲区，用于存储管道送过来的信息
    char *pos = buf;//此时指针，用于存储缓冲区可读部分的首端

    while(1){
        if(read(STD_INPUT,pos,1)<=0){
            //此时表示已经读到了文件末尾
            break;
        }

        if(*pos=='\n'){
            //此时即需要进行一次执行
            (*pos) = '\0';
            /*首先我们将对应的缓冲区数据拷贝到参数列表中*/

            int pid;
            pid = fork();

            //创建子进程用于执行xargs后面的指令
            if(pid == 0){
                arguments[index] = buf;
                exec(argv[1],arguments);
                exit(1);
            }
            else{
                //对于父进程而言，等待子进程运行结束后继续进行即可
                wait(0);
            }
        }

        pos++;//继续指向下一个字符
    }
    
    


    exit(0);
}