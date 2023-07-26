#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
extern pte_t * walk(pagetable_t,uint64,int);
int sys_pgaccess(void)
{
    //获得指定的三个参数
    uint64 base=0;//起始页面地址
    int nums=0;//向后检查的页面数
    uint64 result=0;//存储最终结果
    uint64 buf = 0;//系统调用中存储最终结果，后面需要拷贝到result中
    pagetable_t pagetab;
    struct proc *p = myproc();
    if (argaddr(0, &base) < 0)
        return -1;
    if (argint(1, &nums) < 0)
        return -1;
    if (argaddr(2, &result) < 0)
        return -1;
    if(p==0)
        return 1;

    //设定最大可以进行检索的范围
    if(nums<1)
        return -1;
    
    /*进行加锁*/
    acquire(&p->lock);

    pagetab = p->pagetable;

    for(int i = 0;i<nums;i++){
        pte_t * pte = walk(pagetab,base+i*(uint64)PGSIZE,0);
        if(pte == 0)
        	return -1;
        else if((*pte & PTE_V)==0){
        	return -1;
        }
        else if((*pte & PTE_U)==0){
        	return -1;
        }
        else if(pte!=0&&(PTE_A&(*pte))){
            buf |= (1 <<i);
            (*pte) ^= PTE_A; 
        }
    }
    /*进行解锁*/
    release(&p->lock);

    copyout(pagetab,result,(char*)&buf,((nums-1)/8)+1);
    return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
