#include <stdbool.h>

void __assert_fail( const char* assertion, const char* file, unsigned int line, const char* function )
{
  os_assert_fail( assertion, file, line, function );
}

bool wasm_rt_is_initialized( void )
{
  return true;
}
