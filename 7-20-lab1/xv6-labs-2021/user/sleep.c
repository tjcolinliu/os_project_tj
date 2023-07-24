#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(int argc , char** argv){
	if(argc!=2){
		printf("Total amount of arguments is illegal ! Usage: Sleep [time]\n");
	}
	else{
		int time = atoi(argv[1]);
		sleep(time);
	}
	exit(0);
}
