#pragma once

#include <experimental/bits/simd.h>
#include <immintrin.h>

#include <cassert>

enum class ContentType : uint8_t
{
  Tree,
  Thunk,
  Blob,
  Tag
};

class Handle
{
protected:
  __m256i content_ {};

  Handle() = default;

  Handle( const __m256i val )
    : content_( val )
  {}

  const char* data() const { return reinterpret_cast<const char*>( &content_ ); }

  uint8_t metadata() const { return _mm256_extract_epi8( content_, 31 ); }

  Handle( uint64_t a, uint64_t b, uint64_t c, uint64_t d )
    : content_( __m256i { int64_t( a ), int64_t( b ), int64_t( c ), int64_t( d ) } )
  {}

  Handle( const std::array<char, 32>& input ) { __builtin_memcpy( &content_, input.data(), 32 ); }

public:
  operator __m256i() const { return content_; }

  Handle( std::string hash, size_t size, ContentType content_type )
  {
    assert( hash.size() == 32 );
    hash[31] = static_cast<char>( 0x04 | static_cast<uint8_t>( content_type ) );
    __builtin_memcpy( &content_, hash.data(), 32 );
    content_[2] = size;
  }

  /* Construct a Handle out of literal blob content */
  Handle( std::string_view literal_content )
  {
    assert( literal_content.size() < 32 );
    // set the handle to literal
    uint8_t metadata = 0x20 | literal_content.size();
    __builtin_memcpy( (char*)&content_, literal_content.data(), literal_content.size() );
    __builtin_memcpy( (char*)&content_ + 31, &metadata, 1 );
  }

  std::array<int64_t, 4> get_i64() const { return { content_[0], content_[1], content_[2], content_[3] }; }
};
