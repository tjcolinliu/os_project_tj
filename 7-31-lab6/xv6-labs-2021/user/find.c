#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* const path, char* const path_tail, char* const path_end, const char* const filename)
{
    //打开指定路径
    int fd = 0;
    if ((fd = open(path, 0)) < 0) {
        printf("Err -3 find: cannot open %s\n", path);
        return ;
    }

    //获得指定路径对应统计信息
    struct stat st;
    if (fstat(fd, &st) < 0) {
        printf("Err -4 find: cannot stat %s\n", path);
        close(fd);
        return ;
    }

    if (st.type == T_DIR) {
        *path_tail = '/';

        //不断尝试读取其中信息
        for (struct dirent de; read(fd, &de, sizeof(de)) == sizeof(de); ){

        //如果读到的信息有误，或者读到了./..，则跳过
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }

        //直接写入新地址
        const int name_len = strlen(de.name);
        memmove(path_tail + 1, de.name, name_len);
        path_tail[1 + name_len] = 0;

        //如果找到的话，就打印当前地址
        if (strcmp(filename, de.name) == 0) {
            printf("%s\n", path);
        }

        //如果要递归深度搜索，发现文件夹太深了，就返回错误
        if (path_tail + 1 + name_len + 1 + DIRSIZ + 1 > path_end) {
            printf("find: path too long %s\n", path);
        } else {
            //进行深入递归检索检索
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
  //将地址信息拷贝到缓存区中
  memcpy(buf, argv[1], DIRSIZ);
  find(buf, buf + strlen(argv[1]), buf + 512, argv[2]);
  exit(1);
}
