# bootstrap
This repository builds a compilation toolchain in Fix that compiles WASM
modules to ELF.

## Prerequisite
To build this project, you need to first build
[fix-project/wasm-toolchain](https://github.com/fix-project/wasm-toolchain),
and `wasm-toolchain` needs to be placed at `$ENV{HOME}/wasm-toolchain`.

## Build

### Full Build
```
./build-all.sh
```
After building, there should be a `boot/` and a `.fix/` in the source directory
of the project. Copy both directories to the source directory of Fix.

### Experimental: Incremental Build

Since a full build takes a really long time, it's really helpful to build
bootstrap incrementally.  Unfortunately, traditional build systems don't really
handle the kind of incrementality we need.  In particular, timestamp based
systems like Make or Ninja will always think the fix side of bootstrap needs to
be recompiled, since even if `wasm-to-c-sys` outputs the exact same files,
it'll update the timestamps on those files anyway.

Obviously, running bootstrap within Fix would solve this problem instantly; Fix
will used the cached compilation if it sees matching hashes.  Outside of Fix,
we can achieve similar results using Zach Yedidia's new build system,
[knit](https://github.com/zyedidia/knit), which supports hash-based dynamic
task elision (i.e., the equivalent of memoizing tasks in Fixcache).  This isn't
quite as efficient as building within Fix would be, since Knit can only elide a
task if the output is already on disk in the right location, but is still a
significant improvement in the common case.

To build bootstrap using Knit, add `knit` to your path and run:
```
knit -j $(nproc)
```

This will automatically rebuild the drivers using CMake, then run the
bootstraping process incrementally.

To rebuild a specific file, run:
```
knit [file] -j $(nproc)
```

If you're on stagecast.org, there's a pre-built `knit` binary at
`/opt/knit/bin/knit`.
