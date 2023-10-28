#include <assert.h>
#include <stdbool.h>

#include "wasm-rt.h"

#if WASM_RT_MERGED_OOB_AND_EXHAUSTION_TRAPS
#error "Unsupported: WASM_RT_MERGED_OOB_AND_EXHAUSTION_TRAPS"
#endif

#define abort os_abort

void os_set_signal_handler( void ( * )( bool ) );
WASM_RT_NO_RETURN void os_trap( bool, const char* );
void* os_mmap( size_t, uint8_t );
void os_perror( const char* );
int os_mprotect( void*, size_t, uint8_t );
void* os_calloc( size_t, size_t );
int os_munmap( void*, size_t );
void os_free( void* );
WASM_RT_NO_RETURN void os_abort( void );

static void wasm_rt_signal_handler( bool oob )
{
  if ( oob ) {
    wasm_rt_trap( WASM_RT_TRAP_OOB );
  } else {
    wasm_rt_trap( WASM_RT_TRAP_EXHAUSTION );
  }
#include <assert.h>
}

void wasm_rt_init( void )
{
  os_set_signal_handler( wasm_rt_signal_handler );
}

bool wasm_rt_is_initialized( void )
{
  return true;
}

void wasm_rt_free( void )
{
  os_set_signal_handler( NULL );
}

WASM_RT_NO_RETURN void wasm_rt_trap( wasm_rt_trap_t trap )
{
  bool deterministic;
  switch ( trap ) {
    case WASM_RT_TRAP_EXHAUSTION:
      deterministic = false;
      break;
    case WASM_RT_TRAP_NONE:
    case WASM_RT_TRAP_OOB:
    case WASM_RT_TRAP_INT_OVERFLOW:
    case WASM_RT_TRAP_DIV_BY_ZERO:
    case WASM_RT_TRAP_INVALID_CONVERSION:
    case WASM_RT_TRAP_UNREACHABLE:
    case WASM_RT_TRAP_CALL_INDIRECT:
    case WASM_RT_TRAP_UNCAUGHT_EXCEPTION:
      deterministic = true;
      break;
  }
  os_trap( deterministic, wasm_rt_strerror( trap ) );
};

const char* wasm_rt_strerror( wasm_rt_trap_t trap )
{
  switch ( trap ) {
    case WASM_RT_TRAP_NONE:
      return "No error";
    case WASM_RT_TRAP_OOB:
      return "Out-of-bounds access in linear memory or a table";
    case WASM_RT_TRAP_EXHAUSTION:
      return "Call stack exhausted";
    case WASM_RT_TRAP_INT_OVERFLOW:
      return "Integer overflow on divide or truncation";
    case WASM_RT_TRAP_DIV_BY_ZERO:
      return "Integer divide by zero";
    case WASM_RT_TRAP_INVALID_CONVERSION:
      return "Conversion from NaN to integer";
    case WASM_RT_TRAP_UNREACHABLE:
      return "Unreachable instruction executed";
    case WASM_RT_TRAP_CALL_INDIRECT:
      return "Invalid call_indirect";
    case WASM_RT_TRAP_UNCAUGHT_EXCEPTION:
      return "Uncaught exception";
  }
  return "invalid trap code";
}

void wasm_rt_allocate_memory( wasm_rt_memory_t* memory, uint64_t initial_pages, uint64_t max_pages, bool is64 )
{
  memory->read_only = false;
  if ( initial_pages == 0 ) {
    memory->data = NULL;
    memory->size = 0;
    memory->pages = initial_pages;
    memory->max_pages = max_pages;
    return;
  }

  uint64_t byte_length = initial_pages * WASM_RT_PAGE_SIZE;

  /* Reserve 8GiB. */
  assert( !is64 && "memory64 is not yet compatible with WASM_RT_MEMCHECK_SIGNAL_HANDLER" );
  void* addr = os_mmap( 0x200000000ul, 0 );

  if ( !addr ) {
    os_perror( "os_mmap failed." );
    abort();
  }
  int ret = os_mprotect( addr, byte_length, 0b110 );
  if ( ret != 0 ) {
    os_perror( "os_mprotect failed." );
    abort();
  }
  memory->data = addr;

  memory->size = byte_length;
  memory->pages = initial_pages;
  memory->max_pages = max_pages;
  memory->is64 = is64;
}

extern void wasm_rt_allocate_memory_sw_checked( wasm_rt_memory_t* memory,
                                                uint64_t initial_pages,
                                                uint64_t max_pages,
                                                bool is64 )
{
  memory->read_only = false;
  if ( initial_pages == 0 ) {
    memory->data = NULL;
    memory->size = 0;
    memory->pages = initial_pages;
    memory->max_pages = max_pages;
    return;
  }

  uint64_t byte_length = initial_pages * WASM_RT_PAGE_SIZE;
  memory->data = os_calloc( byte_length, 1 );
  memory->size = byte_length;
  memory->pages = initial_pages;
  memory->max_pages = max_pages;
  memory->is64 = is64;
}

/**
 * Grow a Memory object by `pages`, and return the previous page count. If
 * this new page count is greater than the maximum page count, the grow fails
 * and 0xffffffffu (UINT32_MAX) is returned instead.
 *
 *  ```
 *    wasm_rt_memory_t my_memory;
 *    ...
 *    // Grow memory by 10 pages.
 *    uint32_t old_page_size = wasm_rt_grow_memory(&my_memory, 10);
 *    if (old_page_size == UINT32_MAX) {
 *      // Failed to grow memory.
 *    }
 *  ```
 */
uint64_t wasm_rt_grow_memory( wasm_rt_memory_t*, uint64_t pages );

extern uint64_t wasm_rt_grow_memory_sw_checked( wasm_rt_memory_t*, uint64_t pages );

void wasm_rt_free_memory_hw_checked( wasm_rt_memory_t* memory )
{
  os_munmap( memory->data, memory->size ); // ignore error?
}

void wasm_rt_free_memory_sw_checked( wasm_rt_memory_t* memory )
{
  if ( memory->read_only )
    return;
  os_free( memory->data );
}

/**
 * Initialize a funcref Table object with an element count of `elements` and a
 * maximum size of `max_elements`.
 *
 *  ```
 *    wasm_rt_funcref_table_t my_table;
 *    // 5 elements and a maximum of 10 elements.
 *    wasm_rt_allocate_funcref_table(&my_table, 5, 10);
 *  ```
 */
void wasm_rt_allocate_funcref_table( wasm_rt_funcref_table_t*, uint32_t elements, uint32_t max_elements );

/**
 * Free a funcref Table object.
 */
void wasm_rt_free_funcref_table( wasm_rt_funcref_table_t* );

/**
 * Initialize an externref Table object with an element count
 * of `elements` and a maximum size of `max_elements`.
 * Usage as per wasm_rt_allocate_funcref_table.
 */
void wasm_rt_allocate_externref_table( wasm_rt_externref_table_t*, uint32_t elements, uint32_t max_elements );

/**
 * Free an externref Table object.
 */
void wasm_rt_free_externref_table( wasm_rt_externref_table_t* );

/**
 * Grow a Table object by `delta` elements (giving the new elements the value
 * `init`), and return the previous element count. If this new element count is
 * greater than the maximum element count, the grow fails and 0xffffffffu
 * (UINT32_MAX) is returned instead.
 */
uint32_t wasm_rt_grow_funcref_table( wasm_rt_funcref_table_t*, uint32_t delta, wasm_rt_funcref_t init );
uint32_t wasm_rt_grow_externref_table( wasm_rt_externref_table_t*, uint32_t delta, wasm_rt_externref_t init );
