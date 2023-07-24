#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define STD_INPUT 0

int main(int argc ,char ** argv){
    //�����ʱ��������С��1���򲻽��в���������ʱûָ������Ҫ����ʲôָ��
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

    char buf[1024];//��ʱ��Ӧ�����������ڴ洢�ܵ��͹�������Ϣ
    char *pos = buf;//��ʱָ�룬���ڴ洢�������ɶ����ֵ��׶�

    while(1){
        if(read(STD_INPUT,pos,1)<=0){
            //��ʱ��ʾ�Ѿ��������ļ�ĩβ
            break;
        }

        if(*pos=='\n'){
            //��ʱ����Ҫ����һ��ִ��
            (*pos) = '\0';
            /*�������ǽ���Ӧ�Ļ��������ݿ����������б���*/

            int pid;
            pid = fork();

            //�����ӽ�������ִ��xargs�����ָ��
            if(pid == 0){
                arguments[index] = buf;
                exec(argv[1],arguments);
                exit(1);
            }
            else{
                //���ڸ����̶��ԣ��ȴ��ӽ������н�����������м���
                wait(0);
            }
        }

        pos++;//����ָ����һ���ַ�
    }
    
    


    exit(0);
}