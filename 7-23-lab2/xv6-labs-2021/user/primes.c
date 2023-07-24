#include <stddef.h>
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(int argc, char *argv[]){
    if(argc != 1){
        printf("primes: need no parameter!usage:primes\n");
	exit(-1);
    }

    int w_buf[34];
    int cnt = 0;
    for(int i=2;i<36;i++){
        w_buf[cnt] = i;
        cnt ++;
    }
    while(cnt != 0){
        int fd[2];
        pipe(fd);
        write(fd[1],w_buf,sizeof(w_buf));
        int pid = fork();
        if(pid == 0){
            int r_buf[34];
            read(fd[0],r_buf,sizeof(r_buf));

            // �ǵù�������
            close(fd[0]);
            close(fd[1]);

            int prime = r_buf[0];
            printf("prime %d\n",prime);
            cnt = 0;
            for(int i=1;i<34;i++){
                if(r_buf[i] == 0) break;
                if(r_buf[i] % prime != 0){
                    w_buf[cnt] = r_buf[i];
                    cnt ++;
                }
            }
            //printf("cnt = %d\n",cnt);
            for(int i=cnt;i<36;i++){
                w_buf[i] = 0;
            }
        } else {
            wait(NULL); // ������Ҫ�ȴ��ӽ��̽���
            break;
        }

    }
    exit(0);
}
