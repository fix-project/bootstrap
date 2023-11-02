#ifndef FIXPOINT_SHIMS
#define FIXPOINT_SHIMS

#include <stdint.h>

struct api;
struct os;
struct rt;

struct w2c_fixpoint
{
  void* runtime;
  struct api* api;
  struct os* os;
  struct rt* rt;
};

#define GS __attribute__( ( address_space( 256 ) ) )
/* static struct w2c_fixpoint** const GS context = ( struct w2c_fixpoint * *GS )( 0 ); */

#define CONTEXT ( (struct w2c_fixpoint*)( GET64( 0UL ) ) )
#define SET_CONTEXT( VAL ) ( PUT64( 0UL, (uint64_t)VAL ) )

uint64_t GET64( uint64_t off );
asm( ".globl GET64\n"
     "GET64:\n"
     "  movq    %gs:(%rdi), %rax\n"
     "  ret\n" );

void PUT64( uint64_t off, uint64_t value );
asm( ".globl PUT64\n"
     "PUT64:\n"
     "  movq    %rsi, %gs:(%rdi)\n"
     "  ret\n" );

__m256i fixpoint_get_attached_blob( struct w2c_fixpoint* ctx, wasm_rt_memory_t* mem );

size_t fixpoint_get_memory_size( struct w2c_fixpoint* ctx, wasm_rt_memory_t* mem );

__m256i fixpoint_get_attached_tree( struct w2c_fixpoint* ctx, wasm_rt_externref_table_t* mem );

size_t fixpoint_get_table_size( struct w2c_fixpoint* ctx, wasm_rt_externref_table_t* mem );
#endif
