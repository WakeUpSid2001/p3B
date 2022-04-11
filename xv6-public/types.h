typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
typedef struct ticket_lock_t {
  int next_ticket; // ((aligned (64)));
  int now_serving; // ((aligned (64)));
} lock_t; //((aligned (64)));


