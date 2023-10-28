#ifndef FIXPOINT_SHIMS
#define FIXPOINT_SHIMS

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
struct w2c_fixpoint** const GS context = ( struct w2c_fixpoint * *GS )( 0 );

#endif
