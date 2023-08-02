// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU]; 

void kinit()
{
  for(int i = 0;i<NCPU;i++){
        initlock(&kmem[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  struct run *r;

  //判断需要进行释放的内存块地址是否处于合法区间
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  //将该内存块的所有内容全部置1.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
    
  //此时需要关中断，确保该CPU执行完这段指定
  push_off();

  int p_id = cpuid();
    pop_off();
  acquire(&kmem[p_id].lock);
    
  //此时将该块地址添加到空闲链表中(采用头插法，将该块地址插到链表首部)
  r->next = kmem[p_id].freelist;
  kmem[p_id].freelist = r;

  release(&kmem[p_id].lock);  


}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r = 0;
    
  //此时需要关中断，确保该CPU执行完这段指定
  push_off();
  
  int p_id = cpuid();  
    //关中断
  pop_off();
  acquire(&kmem[p_id].lock);
    
  if(kmem[p_id].freelist){
      //此时代表该CPU对应的空闲链表中存在尚未分配的内存块
      r = kmem[p_id].freelist;
      kmem[p_id].freelist = r->next;
  }else{
      //此时代表该CPU对应的空闲链表中不存在尚未分配的内存块
      //需要我们从别的CPU对应空闲链表中拿一个
      for(int i = 0;i<NCPU;i++){
          if(i == p_id)
              continue;
          acquire(&kmem[i].lock);
          if(kmem[i].freelist){
              //只有在其不为空的时候才能拿
			  r = kmem[i].freelist;
              kmem[i].freelist = r->next;
          	  release(&kmem[i].lock);
              break;
          }
          release(&kmem[i].lock); 
      }
  }

  release(&kmem[p_id].lock);  
    

  
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
