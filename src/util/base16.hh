#pragma once

#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace base16 {

static const std::string base16_chars = "0123456789abcdef";

__attribute__( ( unused ) ) static std::string encode( __m256i name_to_encode )
{
  std::string ret;
  ret.reserve( 64 );

  uint8_t name[32];
  _mm256_storeu_si256( reinterpret_cast<__m256i*>( &name[0] ), name_to_encode );

  for ( size_t i = 0; i < 32; i++ ) {
    ret.push_back( base16_chars[name[i] >> 4] );
    ret.push_back( base16_chars[name[i] & 0xf] );
  }

  return ret;
}
}
