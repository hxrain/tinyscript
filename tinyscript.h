#ifndef TINYSCRIPT_H
#define TINYSCRIPT_H

#include <stdint.h>

#ifdef __propeller__
// define SMALL_PTRS to use 16 bits for pointers
// useful for machines with <= 64KB of RAM
#define SMALL_PTRS
#endif

// errors
// all the ParseXXX functions return 0 on success, a non-zero
// error code otherwise
enum {
    TS_ERR_OK = 0,
    TS_ERR_NOMEM = -1,
    TS_ERR_SYNTAX = -2,
    TS_ERR_UNKNOWN_SYM = -3,
    TS_ERR_OK_ELSE = 1, // special internal condition
};

// we use this a lot
typedef char Byte;

//our target is machines with < 64KB of memory, so 16 bit pointers
//will do
typedef Byte *Ptr;

// val has to be able to hold a pointer
typedef intptr_t Val;

// strings are represented as (length,ptr) pairs
// this is done so that we can re-use variable names and similar
// substrings directly from the script text, without having
// to insert 0 into them
typedef struct {
#ifdef SMALL_PTRS
    uint16_t len_;
    uint16_t ptr_;
#else
    unsigned len_;
    const char *ptr_;
#endif
} String;

static inline unsigned StringGetLen(String s) { return (unsigned)s.len_; }
static inline const char *StringGetPtr(String s) { return (const char *)(intptr_t)s.ptr_; }
#ifdef SMALL_PTRS
static inline void StringSetLen(String *s, unsigned len) { s->len_ = (uint16_t)len; }
static inline void StringSetPtr(String *s, const char *ptr) { s->ptr_ = (uint16_t)(intptr_t)ptr; }
#else
static inline void StringSetLen(String *s, unsigned len) { s->len_ = len; }
static inline void StringSetPtr(String *s, const char *ptr) { s->ptr_ = ptr; }
#endif

// symbols can take the following forms:
#define INT      0x0  // integer
#define STRING   0x1  // string
#define PROC     0x2  // scripting procedure
#define TOKEN    0x3  // language token
#define OPERATOR 0x4  // operator; precedence in high 8 bits
#define BUILTIN  0x5  // builtin: number of operands in high 8 bits

#define BINOP(x) (((x)<<8)+OPERATOR)
#define CFUNC(x) (((x)<<8)+BUILTIN)

typedef struct symbol {
    String name;
    int    type;   // symbol type
    Val    value;  // symbol value, or string ptr
    Val    aux;    // string len for string values
} Sym;

#define MAX_BUILTIN_PARAMS 4

typedef Val (*Cfunc)(Val, Val, Val, Val);
typedef Val (*Opfunc)(Val, Val);

//
// global interface
//
int TinyScript_Init(void *mem, int mem_size);
int TinyScript_Define(const char *name, int toktype, Val value);
int TinyScript_Run(const char *s, int saveStrings, int topLevel);

// provided by our caller
extern int inchar(void);
extern void outchar(int c);

#endif
