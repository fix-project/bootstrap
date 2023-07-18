#include <iterator>

namespace std {

inline namespace __2 {

template<class _CharT, class _Traits>
class basic_ifstream
{
public:
  basic_ifstream( const char* str [[maybe_unused]] ) {}
  bool good() { return true; }
  bool is_open() { return true; }
  operator std::istreambuf_iterator<_CharT>() { return {}; }
};

template<class CharT, class Traits, class Allocator>
inline std::basic_ifstream<CharT, Traits>& getline( std::basic_ifstream<CharT, Traits>& input,
                                                    std::basic_string<CharT, Traits, Allocator>& str )
{
  return input;
}

}

}
