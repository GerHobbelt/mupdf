
/* CONFIG_ALL_UNICODE - Define this if you want all-Unicode support */
#define CONFIG_ALL_UNICODE 1

/* CONFIG_ATOMICS - Define this if you want 'Atomics' object [https://tc39.es/ecma262/#sec-atomics-object] */
#define CONFIG_ATOMICS 1

/* CONFIG_BIGNUM - Define this if you want BigNum support */
#define CONFIG_BIGNUM 1

/* CONFIG_PRINTF_RNDN - define it if printf uses the RNDN rounding mode instead of RNDNA */
#define CONFIG_PRINTF_RNDN 1

/* CONFIG_CHECK_JSVALUE - define it if printf uses the RNDN rounding mode instead of RNDNA */
#define CONFIG_CHECK_JSVALUE 1

/* CONFIG_DEBUGGER - define it if you want debug protocol support */
#define CONFIG_DEBUGGER 1

/* HAVE_MALLOC_USABLE_SIZE - Define this if you have a working malloc_usable_size() function */
/* #undef HAVE_MALLOC_USABLE_SIZE */

/* PROFILE - Define this if you want to profile the code */
#define PROFILE 1

/* USE_WORKER - Define this if you want to add support for a worker thread */
#define USE_WORKER 1

/* USE_BIGNUM - Enable BigFloat, BigDecimal by default */
#define USE_BIGNUM 1


// ------------------------- configuration -------------------------------------

#if !defined(QJS_NO_DUMP)

/* DUMP_ATOMS - Define this if you want to dump atoms when freeing context */
#define DUMP_ATOMS 1

/* DUMP_SHAPES - Define this if you want to dump shapes when freeing context */
#define DUMP_SHAPES 1

/* DUMP_OBJECTS - Define this if you want to dump objects when freeing context */
#define DUMP_OBJECTS 1

/* DUMP_MEM - Define this if you want to dump memory when freeing context */
#define DUMP_MEM 1

/* DUMP_CLOSURE  */
#define DUMP_CLOSURE 1

/* DUMP_BYTECODE - Define this if you want to be able to dump bytecode */
/* dump the bytecode of the compiled functions: combination of bits
   1: dump pass 3 final byte code
   2: dump pass 2 code
   4: dump pass 1 code
   8: dump stdlib functions
  16: dump bytecode in hex
  32: dump line number table
  64: dump compute_stack_size
 */
#define DUMP_BYTECODE 0x3F

 /* DUMP_FREE - Define this if you want to dump a message on freeing objects */
#define DUMP_FREE 1

/* DUMP_GC - Define this if you want to dump a message on garbage collection */
#define DUMP_GC 1

/* DUMP_GC_FREE - Define this if you want to dump a message when the garbage collector frees a resource */
#define DUMP_GC_FREE 1

/* DUMP_LEAKS - Define this if you want to dump memory leaks; set to compile-time DUMP_LEAKS to 2 for more dumping */
#define DUMP_LEAKS 2

/* DUMP_MODULE_RESOLVE - Define this if you want to debug module resolution */
#define DUMP_MODULE_RESOLVE 1

/* DUMP_PROMISE - Define this if you want to debug promises */
#define DUMP_PROMISE 1

/* DUMP_READ_OBJECT - Define this if you want to debug binary object reader */
#define DUMP_READ_OBJECT 1

/* dump object free */
//#define DUMP_FREE
//#define DUMP_CLOSURE
/* dump the bytecode of the compiled functions: combination of bits
   1: dump pass 3 final byte code
   2: dump pass 2 code
   4: dump pass 1 code
   8: dump stdlib functions
  16: dump bytecode in hex
  32: dump line number table
 */
 //#define DUMP_BYTECODE  (1)
 /* dump the occurrence of the automatic GC */
 //#define DUMP_GC
 /* dump objects freed by the garbage collector */
 //#define DUMP_GC_FREE
 /* dump objects leaking when freeing the runtime */
//#define DUMP_LEAKS  1
/* dump memory usage before running the garbage collector */
//#define DUMP_MEM
//#define DUMP_OBJECTS    /* dump objects in JS_FreeContext */
//#define DUMP_ATOMS      /* dump atoms in JS_FreeContext */
//#define DUMP_SHAPES     /* dump shapes in JS_FreeContext */
//#define DUMP_MODULE_RESOLVE
//#define DUMP_PROMISE
//#define DUMP_READ_OBJECT

#endif
