#define UPROC_NAME(name) . name = uproc_##name ,
#define MAP_SHARED    1
#define MAP_PRIVATE   2
#define MAP_UNMAP     3

#define PROT_NONE   0x1
#define PROT_READ   0x2
#define PROT_WRITE  0x4

#define MMAP_ALL (MMAP_READ | MMAP_WRITE)

#include "initcode.inc"

extern task_t * current_all[8];
extern task_t * task_all_pid[32768];

extern unsigned char _init[];
extern unsigned int _init_len;
