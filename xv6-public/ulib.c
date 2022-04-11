#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "stddef.h"

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

void* arrayStack[64];
int pidArr[64];

// typedef struct ticket_lock {
  // volatile unsigned short next_ticket; // ((aligned (64)));
  //volatile unsigned short now_serving; // ((aligned (64)));
//} lock_t; //((aligned (64)));

static inline int fetchAndAdd(int* variable, int value)
{
  __asm__ volatile("lock; xaddl %0, %1"
    : "+r" (value), "+m" (*variable) // input + output
    : // No input-only
    : "memory"
  );
  return value;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

void lock_init(lock_t *lock)
{
  lock->now_serving = 0;
  lock->next_ticket = 0;
}

void lock_acquire(lock_t *lock)
{
  int turn;
  turn = fetchAndAdd(&lock->next_ticket, 1);
  while(fetchAndAdd(&lock->now_serving, 0) != turn);
}

int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2)
{
  int PGSIZE = 4096;
  void *stack;
  void *freep;
  freep = (void*)malloc(2*PGSIZE);

  if(freep == 0){
	  return -1;
  }
 
  //stack = freep;
	
  /*for(int i = 0; i < 64; i++){
	  if(arrayStack[i] == NULL){
		  arrayStack[i] = stack;
		  break;
	  }
  }*/

  if((uint)freep % PGSIZE == 0){
	  stack = freep;
  }
  else{
	  stack = freep + (PGSIZE - ((uint)freep % PGSIZE));
  }

 int returnedVal = clone(start_routine, arg1, arg2, stack);

 /*for(int i = 0; i < 64; i++){
	  if(arrayStack[i] == NULL){
		  if(pidArr[i] == returnedVal){
		  	arrayStack[i] = stack;
		  	break;
		  }
	  }
 }*/
  
  
  //int returnedVal = clone(start_routine, arg1, arg2, stack);
  
  return returnedVal; 
}

int thread_join()
{
  // int PGSIZE = 4096;
  void *stack;
  //stack = (void*)malloc(2*PGSIZE);

  int returnedVal = join(&stack);
  free(stack);

  /*for(int i = 0; i < 64; i++){
	if(arrayStack[i] != NULL){
  		if(pidArr[i] == returnedVal){
			pidArr[i] = 0;
			// free(arrayStack[i])
			arrayStack[i] = NULL;
			break;
		}
	}
  }*/



  return returnedVal;
}

void lock_release(lock_t *lock)
{
  lock->now_serving += 1;
  //fetchAndAdd(&lock->now_serving, 0);
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
