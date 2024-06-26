const char* cc1args[] = { "-ferror-limit",
                          "19",
                          "-Wall",
                          "-Werror",
                          "-mllvm",
                          "-treat-scalable-fixed-error-as-warning",
                          "-disable-free",
                          "-fsyntax-only",
                          "-x",
                          "c",
                          "function.c",
                          "-target-cpu",
                          "znver2",
                          "-tune-cpu",
                          "znver2",
                          "-target-feature",
                          "-tsxldtrk",
                          "-target-feature",
                          "+cx16",
                          "-target-feature",
                          "+sahf",
                          "-target-feature",
                          "-tbm",
                          "-target-feature",
                          "-avx512ifma",
                          "-target-feature",
                          "+sha",
                          "-target-feature",
                          "+crc32",
                          "-target-feature",
                          "-fma4",
                          "-target-feature",
                          "-vpclmulqdq",
                          "-target-feature",
                          "+prfchw",
                          "-target-feature",
                          "+bmi2",
                          "-target-feature",
                          "-cldemote",
                          "-target-feature",
                          "+fsgsbase",
                          "-target-feature",
                          "-ptwrite",
                          "-target-feature",
                          "-amx-tile",
                          "-target-feature",
                          "-uintr",
                          "-target-feature",
                          "-gfni",
                          "-target-feature",
                          "+popcnt",
                          "-target-feature",
                          "-widekl",
                          "-target-feature",
                          "+aes",
                          "-target-feature",
                          "-avx512bitalg",
                          "-target-feature",
                          "-movdiri",
                          "-target-feature",
                          "+xsaves",
                          "-target-feature",
                          "-avxvnni",
                          "-target-feature",
                          "-avx512fp16",
                          "-target-feature",
                          "-avx512vnni",
                          "-target-feature",
                          "-amx-bf16",
                          "-target-feature",
                          "-avx512vpopcntdq",
                          "-target-feature",
                          "-pconfig",
                          "-target-feature",
                          "+clwb",
                          "-target-feature",
                          "-avx512f",
                          "-target-feature",
                          "+xsavec",
                          "-target-feature",
                          "+clzero",
                          "-target-feature",
                          "-pku",
                          "-target-feature",
                          "+mmx",
                          "-target-feature",
                          "-lwp",
                          "-target-feature",
                          "+rdpid",
                          "-target-feature",
                          "-xop",
                          "-target-feature",
                          "+rdseed",
                          "-target-feature",
                          "-waitpkg",
                          "-target-feature",
                          "-kl",
                          "-target-feature",
                          "-movdir64b",
                          "-target-feature",
                          "+sse4a",
                          "-target-feature",
                          "-avx512bw",
                          "-target-feature",
                          "+clflushopt",
                          "-target-feature",
                          "+xsave",
                          "-target-feature",
                          "-avx512vbmi2",
                          "-target-feature",
                          "+64bit",
                          "-target-feature",
                          "-avx512vl",
                          "-target-feature",
                          "-serialize",
                          "-target-feature",
                          "-hreset",
                          "-target-feature",
                          "-invpcid",
                          "-target-feature",
                          "-avx512cd",
                          "-target-feature",
                          "+avx",
                          "-target-feature",
                          "-vaes",
                          "-target-feature",
                          "-avx512bf16",
                          "-target-feature",
                          "+cx8",
                          "-target-feature",
                          "+fma",
                          "-target-feature",
                          "-rtm",
                          "-target-feature",
                          "+bmi",
                          "-target-feature",
                          "-enqcmd",
                          "-target-feature",
                          "+rdrnd",
                          "-target-feature",
                          "+mwaitx",
                          "-target-feature",
                          "+sse4.1",
                          "-target-feature",
                          "+sse4.2",
                          "-target-feature",
                          "+avx2",
                          "-target-feature",
                          "+fxsr",
                          "-target-feature",
                          "+wbnoinvd",
                          "-target-feature",
                          "+sse",
                          "-target-feature",
                          "+lzcnt",
                          "-target-feature",
                          "+pclmul",
                          "-target-feature",
                          "+f16c",
                          "-target-feature",
                          "+ssse3",
                          "-target-feature",
                          "-sgx",
                          "-target-feature",
                          "-shstk",
                          "-target-feature",
                          "+cmov",
                          "-target-feature",
                          "-avx512vbmi",
                          "-target-feature",
                          "-amx-int8",
                          "-target-feature",
                          "+movbe",
                          "-target-feature",
                          "-avx512vp2intersect",
                          "-target-feature",
                          "+xsaveopt",
                          "-target-feature",
                          "-avx512dq",
                          "-target-feature",
                          "+sse2",
                          "-target-feature",
                          "+adx",
                          "-target-feature",
                          "+sse3",
                          "-triple",
                          "x86_64-pc-linux-gnu",
                          "-resource-dir",
                          "lib/clang/16.0.0",
                          "-isystem",
                          "/usr/lib/llvm-16/lib/clang/16.0.0/include/",
                          "-internal-externc-isystem",
                          "/usr/include/x86_64-linux-gnu",
                          "-internal-externc-isystem",
                          "/include",
                          "-internal-externc-isystem",
                          "/usr/include",
                          "-std=gnu17",
                          "-fmath-errno",
                          "-pic-level",
                          "2",
                          "-pic-is-pie",
                          "-Os",
                          "-fgnuc-version=4.2.1",
                          "-ffp-contract=on",
                          "-fno-experimental-relative-c++-abi-vtables",
                          "-fdebug-compilation-dir=/home/yuhan/fixpoint",
                          "-fcoverage-compilation-dir=/home/yuhan/fixpoint",
                          "-faddrsig",
                          "-debugger-tuning=gdb",
                          "-disable-llvm-verifier",
                          "-funwind-tables=2",
                          "-mconstructor-aliases",
                          "-vectorize-loops",
                          "-vectorize-slp",
                          "-clear-ast-before-backend",
                          "-discard-value-names",
                          "-main-file-name",
                          "function.c",
                          "-finline-functions",
                          "-save-temps=obj",
                          "-fdiagnostics-hotness-threshold=0",
                          "-D",
                          "__GCC_HAVE_DWARF2_CFI_ASM=1" };
