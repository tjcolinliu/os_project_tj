#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(){
    int pip1[2] = {};
    int pip2[2] = {};

    //ע��һ���ܵ����˿�pip1[0]��ʾ���룬pip1[1]��ʾд��
    if(pipe(pip1) < 0){
        //��ʱ�������ܵ�ʧ��
        printf("Error 201 : Failed to allocate pipeline!");
        exit(-201);
    }
        //ע��һ���ܵ����˿�pip2[0]��ʾ���룬pip2[1]��ʾд��
    if(pipe(pip2) < 0){
        //��ʱ�������ܵ�ʧ��
        printf("Error 202 : Failed to allocate pipeline!");
        exit(-202);
    }
    
    int forkPid = fork();

    if(forkPid == 0){
        //��ʱ����������ӽ���
        char buf[1];
        read(pip1[0],buf,1);
        close(pip1[0]);
        printf("%d: received ping\n",getpid());
        write(pip2[1],buf,1);
        close(pip2[1]);

    }
    else{
        //��ʱ��������˸�����
        char buf[1] = {'a'};
        //��ܵ���д������
        write(pip1[1],buf,1);
        close(pip1[1]);
        read(pip2[0],buf,1);
        close(pip2[0]);
        printf("%d: received pong\n",getpid());
    }


    exit(0);
}
