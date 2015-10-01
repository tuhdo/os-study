/* width of stack == width of int */
#define	STACKITEM	int

/* round up width of objects pushed on stack. The expression before the
   & ensures that we get 0 for objects of size 0. */
#define	VA_SIZE(TYPE)                           \
    ((sizeof(TYPE) + sizeof(STACKITEM) - 1)     \
     & ~(sizeof(STACKITEM) - 1))
