#pragma once

#include <experimental/bits/simd.h>
#include <immintrin.h>

#include <cassert>

enum class ContentType : uint8_t
{
  Tree,
  Thunk,
  Blob
};

class Name
{
protected:
  __m256i content_ {};

  Name() = default;

  Name( const __m256i val )
    : content_( val )
  {}


  const char* data() const { return reinterpret_cast<const char*>( &content_ ); }

  uint8_t metadata() const { return _mm256_extract_epi8( content_, 31 ); }

  Name( uint64_t a, uint64_t b, uint64_t c, uint64_t d )
    : content_( __m256i { int64_t( a ), int64_t( b ), int64_t( c ), int64_t( d ) } )
  {}

  Name( const std::array<char, 32>& input ) { __builtin_memcpy( &content_, input.data(), 32 ); }

public:
  operator __m256i() const { return content_; }

  Name( std::string hash, size_t size, ContentType content_type )
  {
    assert( hash.size() == 32 );
    hash[31] = static_cast<char>( 0x04 | static_cast<uint8_t>( content_type ) );
    __builtin_memcpy( &content_, hash.data(), 32 );
    content_[2] = size;
  }
};
