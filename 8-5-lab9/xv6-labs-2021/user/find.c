#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* const path, char* const path_tail, char* const path_end, const char* const filename)
{
    //��ָ��·��
    int fd = 0;
    if ((fd = open(path, 0)) < 0) {
        printf("Err -3 find: cannot open %s\n", path);
        return ;
    }

    //���ָ��·����Ӧͳ����Ϣ
    struct stat st;
    if (fstat(fd, &st) < 0) {
        printf("Err -4 find: cannot stat %s\n", path);
        close(fd);
        return ;
    }

    if (st.type == T_DIR) {
        *path_tail = '/';

        //���ϳ��Զ�ȡ������Ϣ
        for (struct dirent de; read(fd, &de, sizeof(de)) == sizeof(de); ){

        //�����������Ϣ���󣬻��߶�����./..��������
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }

        //ֱ��д���µ�ַ
        const int name_len = strlen(de.name);
        memmove(path_tail + 1, de.name, name_len);
        path_tail[1 + name_len] = 0;

        //����ҵ��Ļ����ʹ�ӡ��ǰ��ַ
        if (strcmp(filename, de.name) == 0) {
            printf("%s\n", path);
        }

        //���Ҫ�ݹ���������������ļ���̫���ˣ��ͷ��ش���
        if (path_tail + 1 + name_len + 1 + DIRSIZ + 1 > path_end) {
            printf("find: path too long %s\n", path);
        } else {
            //��������ݹ��������
            find(path, path_tail + 1 + name_len, path_end, filename);
        }
        }
    }

    close(fd);
    return;
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Err -1:Argument fault ! usage: find path filename\n");
    exit(-1);
  }
  char buf[512] = {0};
  //����ַ��Ϣ��������������
  memcpy(buf, argv[1], DIRSIZ);
  find(buf, buf + strlen(argv[1]), buf + 512, argv[2]);
  exit(1);
}
