#include <vector>

static const std::vector<const char*> system_deps
  = { "/usr/include/math.h",
      "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h",
      "/usr/include/features.h",
      "/usr/include/features-time64.h",
      "/usr/include/x86_64-linux-gnu/bits/wordsize.h",
      "/usr/include/x86_64-linux-gnu/bits/timesize.h",
      "/usr/include/stdc-predef.h",
      "/usr/include/x86_64-linux-gnu/sys/cdefs.h",
      "/usr/include/x86_64-linux-gnu/bits/long-double.h",
      "/usr/include/x86_64-linux-gnu/gnu/stubs.h",
      "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h",
      "/usr/include/x86_64-linux-gnu/bits/types.h",
      "/usr/include/x86_64-linux-gnu/bits/typesizes.h",
      "/usr/include/x86_64-linux-gnu/bits/time64.h",
      "/usr/include/x86_64-linux-gnu/bits/math-vector.h",
      "/usr/include/x86_64-linux-gnu/bits/libm-simd-decl-stubs.h",
      "/usr/include/x86_64-linux-gnu/bits/floatn.h",
      "/usr/include/x86_64-linux-gnu/bits/floatn-common.h",
      "/usr/include/x86_64-linux-gnu/bits/flt-eval-method.h",
      "/usr/include/x86_64-linux-gnu/bits/fp-logb.h",
      "/usr/include/x86_64-linux-gnu/bits/fp-fast.h",
      "/usr/include/x86_64-linux-gnu/bits/mathcalls-helper-functions.h",
      "/usr/include/x86_64-linux-gnu/bits/mathcalls.h",
      "/usr/include/string.h",
      "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/__locale_t.h",
      "/usr/include/strings.h",
      "/usr/include/assert.h",
      "/usr/include/alloca.h",
      "/usr/include/stdlib.h",
      "/usr/include/x86_64-linux-gnu/bits/waitflags.h",
      "/usr/include/x86_64-linux-gnu/bits/waitstatus.h",
      "/usr/include/x86_64-linux-gnu/sys/types.h",
      "/usr/include/x86_64-linux-gnu/bits/types/clock_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/clockid_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/time_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/timer_t.h",
      "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h",
      "/usr/include/x86_64-linux-gnu/bits/stdint-least.h",
      "/usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h",
      "/usr/include/endian.h",
      "/usr/include/x86_64-linux-gnu/bits/endian.h",
      "/usr/include/x86_64-linux-gnu/bits/endianness.h",
      "/usr/include/x86_64-linux-gnu/bits/byteswap.h",
      "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h",
      "/usr/include/x86_64-linux-gnu/sys/select.h",
      "/usr/include/x86_64-linux-gnu/bits/select.h",
      "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/__sigset_t.h",
      "/usr/include/x86_64-linux-gnu/bits/types/struct_timeval.h",
      "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h",
      "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h",
      "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h",
      "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h",
      "/usr/include/x86_64-linux-gnu/bits/atomic_wide_counter.h",
      "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h",
      "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h",
      "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h",
      "/usr/include/stdint.h",
      "/usr/include/x86_64-linux-gnu/bits/wchar.h",
      "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h",
      "/usr/include/setjmp.h",
      "/usr/include/x86_64-linux-gnu/bits/setjmp.h",
      "/usr/include/x86_64-linux-gnu/bits/types/struct___jmp_buf_tag.h" };

static const std::vector<const char*> clang_deps
  = { "/usr/lib/llvm-16/lib/clang/16.0.0/include/immintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/adcintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/x86gprintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/usermsrintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/hresetintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/uintrintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/crc32intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/prfchiintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/raointintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/mmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xmmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/mm_malloc.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/stdarg.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg_va_arg.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg___va_copy.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg_va_copy.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg_va_list.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg___gnuc_va_list.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stdarg_header_macro.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/stddef.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_max_align_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_ptrdiff_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_size_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_rsize_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_wchar_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_null.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_nullptr_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_unreachable.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_max_align_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_offsetof.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_wint_t.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__stddef_header_macro.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/emmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/pmmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/tmmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/smmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/popcntintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/wmmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__wmmintrin_aes.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/__wmmintrin_pclmul.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/clflushoptintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/clwbintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx2intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/f16cintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/bmiintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/bmi2intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/lzcntintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/fmaintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512fintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512bwintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512bitalgintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512cdintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vpopcntdqintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vpopcntdqvlintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vnniintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlvnniintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxvnniintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512dqintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlbitalgintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlbwintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlcdintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vldqintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512ifmaintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512ifmavlintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxifmaintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vbmiintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vbmivlintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vbmi2intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlvbmi2intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512fp16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlfp16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512bf16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlbf16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/pkuintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/vpclmulqdqintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/vaesintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/gfniintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxvnniint8intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxneconvertintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/sha512intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/sm3intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/sm4intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avxvnniint16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/amxcomplexintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/rtmintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xtestintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/shaintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/fxsrintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xsaveintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xsaveoptintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xsavecintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/xsavesintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/cetintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/adxintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/rdseedintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/wbnoinvdintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/cldemoteintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/waitpkgintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/movdirintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/pconfigintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/sgxintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/ptwriteintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/invpcidintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/amxfp16intrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/cmpccxaddintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/keylockerintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/amxintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vp2intersectintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/avx512vlvp2intersectintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/enqcmdintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/serializeintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/tsxldtrkintrin.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/stdint.h",
      "/usr/lib/llvm-16/lib/clang/16.0.0/include/stdbool.h" };
