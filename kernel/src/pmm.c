#include <common.h>

#define MAX_malloc (16*1024*1024)
#define Unit_size (MAX_malloc)
#define Unit_mask (-Unit_size)

#define HEAP_START ROUNDUP((uintptr_t)heap.start,Unit_size)
#define HEAP_END ROUNDDOWN((uintptr_t)heap.end,Unit_size)

#define LOWBIT(x) ((x)&((x)^((x)-1)))

#define MAGIC_UNUSED (0x7c)
#define MAGIC_UNLOCKED (0)
#define MAGIC_LOCKED (1)
#define MAGIC_MHD (0x19810114)

#define LOCK_ADDR(x) ((x)+Unit_size-sizeof(int))

typedef int spinlock_t;

typedef struct __free_list{
  uintptr_t size;
  struct __free_list *nxt;
}free_list;

typedef struct{
  uintptr_t size;
  uintptr_t magic;
}mem_tag;

static inline void spin_lock(spinlock_t *lk) {
  while (1) {
    intptr_t value = atomic_xchg(lk, MAGIC_LOCKED);
    if (value == MAGIC_UNLOCKED) {
      break;
    }
  }
}

static inline void spin_unlock(spinlock_t *lk) {
  atomic_xchg(lk, MAGIC_UNLOCKED);
}

static int lock;

static inline void * kernel_alloc(size_t len){
  static uintptr_t sbrk_now=0;
  if(!sbrk_now) sbrk_now=HEAP_END;
  sbrk_now-=len;
  return sbrk_now;
}

static inline size_t up_bound(size_t size){
  int i=0;
  while((1<<i)<size) ++i;
  return i;
}

free_list * start_of_128;

void init_128(){
  start_of_128=kernel_alloc(sizeof(free_list *));

}

void init_

void init_mm(){
  

/*  num_of_block=(HEAP_END-HEAP_START)/Unit_size;
  for(uintptr_t i=HEAP_START,j=0;i<HEAP_END;i+=Unit_size,j++){
//    printf("%lx,%lx,i=%lx,j=%d,num=%d\n",HEAP_USE_START,HEAP_END,i,j,num_of_block);
    *lock_addr(j)=MAGIC_LOCKED;
    start_of_free_list(j)=(free_list *)i;
    memset((void *)i,MAGIC_UNUSED,Unit_size);
    ((free_list *)i)->size=Unit_size;
    ((free_list *)i)->nxt=NULL;
    *lock_addr(j)=MAGIC_UNLOCKED;
  }*/
}

#ifndef TEST
static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  init_mm();
//  init_mm(begin=(((uintptr_t)heap.end-Unit_size+1)&Unit_mask)+Unit_size,end=(uintptr_t)heap.end&Unit_mask);
  return;
}
#else
#define HEAP_SIZE 0x40000000
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  heap.start = ptr;
  heap.end   = ptr + HEAP_SIZE;
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
  init_mm();
  printf("Initialize memory Completed!\n");
//  init_mm(begin=(((uintptr_t)heap.end-Unit_size+1)&Unit_mask)+Unit_size,end=(uintptr_t)heap.end&Unit_mask);
  return;
}
#endif

static inline void * find(size_t len){

  return NULL;
}

static inline void work(void * ptr,size_t len){
  mem_tag * pos=(mem_tag *)((uintptr_t)ptr+len-sizeof(mem_tag));
  pos->size=len;
  pos->magic=MAGIC_MHD;
  return;
}

static inline void * kalloc_case3(size_t size){
  uintptr_t ret=0;
  spin_lock(&lock);

  spin_unlock(&lock);
  return (void *)ret;
}

static void * kalloc(size_t size){
  if(size>MAX_malloc) return NULL;
  return kalloc_case3(size);

/*  size=up_bound(size+sizeof(mem_tag));
  uintptr_t try_num=0;
  for(uintptr_t i=0,j=HEAP_START;try_num<num_of_block;i=(i+1)%num_of_block,j=(j+Unit_size==HEAP_END?HEAP_START:j+HEAP_END))
  if(atomic_xchg(lock_addr(i),MAGIC_LOCKED)==MAGIC_UNLOCKED){
    ++try_num;
    void * ret=find(size);
    if(ret) work(ret,size);
    spin_unlock(lock_addr(i));
    if(ret) return ret;
  }
  return NULL;*/
}

static inline void real_free(uintptr_t ptr){
  spin_lock(&lock);
  /*uintptr_t len=LOWBIT(ptr);
  for(;len;len>>=1){
    uintptr_t pos=ptr+len-sizeof(mem_tag);
    if(((mem_tag *)pos)->size+ptr==pos+sizeof(mem_tag)&&((mem_tag *)pos)->magic==MAGIC_MHD) break;
  }
  memset((void *)ptr,len,MAGIC_UNUSED);*/

}

static void kfree(void * ptr){
  uintptr_t pos=(ROUNDDOWN((uintptr_t)ptr,Unit_size)-HEAP_START)/Unit_size;
  spin_lock(lock_addr(pos));
  real_free((uintptr_t)ptr);
  spin_unlock(lock_addr(pos));
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};