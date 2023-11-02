#include <stdbool.h>

bool wasm_rt_is_initialized( void )
{
  return true;
}

__m256i fixpoint_get_attached_blob( struct w2c_fixpoint* ctx, wasm_rt_memory_t* mem )
{
  return mem->ref;
}

size_t fixpoint_get_memory_size( struct w2c_fixpoint* ctx, wasm_rt_memory_t* mem )
{
  return mem->size;
}

__m256i fixpoint_get_attached_tree( struct w2c_fixpoint* ctx, wasm_rt_externref_table_t* mem )
{
  return mem->ref;
}

size_t fixpoint_get_table_size( struct w2c_fixpoint* ctx, wasm_rt_externref_table_t* mem )
{
  return mem->size;
}
