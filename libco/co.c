#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <assert.h>
#include <stdint.h>

#ifdef LOCAL_MACHINE
  #include <stdio.h>
  #define DEBUG(...) printf(__VA_ARGS__)
#else
  #define DEBUG()
#endif

#define CAO_DEBUG(name) DEBUG("[%3d:%10s]%s!\n",__LINE__,__FUNCTION__,name)

typedef enum{
  CO_NEW=1,
  CO_RUNNING,
  CO_WAITING,
  CO_DEAD,
}co_status;

#define STACK_SIZE (64*1024+32)
struct co {
  const char * name;
  void (*func) (void *);
  void * arg;

  co_status status;
  struct co * waiter;
  struct co * pre;
  struct co * nxt;
  jmp_buf context;
  uint8_t stack[STACK_SIZE] ;
};

struct co * st=NULL;

static struct co * current=NULL;

static void add_list(struct co * x){
  x->nxt=st->nxt;st->nxt=x;
  x->nxt->pre=x;x->pre=st;
  return;
}

static void del_list(struct co * x){
  x->nxt->pre=x->pre;x->pre->nxt=x->nxt;
  free(x);
  return;
}

static void * malloc__(size_t size){
  void * ret=malloc(size);
  while (ret==NULL) ret=malloc(size);
  return ret;
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  struct co *ret = malloc__(sizeof(struct co));
  ret->name=name;ret->func=func;ret->arg=arg;ret->waiter=NULL;
  add_list(ret);
  ret->status=CO_NEW;
  return ret;
}

static void __co_yield();

static void start(){
  assert(current->status==CO_NEW);
  current->status=CO_RUNNING;
  current->func(current->arg);
  current->status=CO_DEAD;
  if(current->waiter) {
    assert(current->waiter->status==CO_WAITING);
    current->waiter->status=CO_RUNNING;
  }
  __co_yield();
}

static void stack_switch_call(void * sp, void *entry);

static void __co_yield(){
  for(current=current->nxt;current->status!=CO_RUNNING&&current->status!=CO_NEW;current=current->nxt);

  switch(current->status){
    case CO_NEW: 
      stack_switch_call(current->stack+STACK_SIZE,start);
      assert(0);//should not reach here!
    case CO_RUNNING: 
      longjmp(current->context,1);
      assert(0);//should not reach here!
    default: assert(0);
  }
}

static inline void stack_switch_call(void * sp, void *entry) {
  sp=(void *)( ((uintptr_t) sp & -16) );
//  DEBUG("%p %p %p\n",(void *)sp,entry,(void *)arg);
  asm volatile (
#if __x86_64__
    "movq %0, %%rsp; call *%1"
      : : "b"((uintptr_t)sp), "d"(entry) : "memory"
#else
    "movl %0, %%esp; call *%1"
      : : "b"((uintptr_t)sp - 8), "d"(entry) : "memory"
#endif
  );
  assert(0);//should not reach here!
}

void co_wait(struct co *co) {
//  CAO_DEBUG(co->name);
  assert(co);assert(co->waiter==NULL);assert(current->status==CO_RUNNING);
  if(co->status!=CO_DEAD){
    current->status=CO_WAITING;
    co->waiter=current;
    if(!setjmp(current->context)) __co_yield();
    assert(co->status==CO_DEAD&&current->status==CO_RUNNING);
  }
  del_list(co);
  return;
}

void co_yield() {
  int val=setjmp(current->context);
  if(!val) __co_yield();
  return;
}

void __attribute__((constructor)) init(){
  current=malloc__(sizeof(struct co));
  st=current;current->pre=current->nxt=current;
  current->name="main";
  current->status=CO_RUNNING;  
}

void __attribute__ ((destructor)) free_main_malloc(){
  free(st);
}