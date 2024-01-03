(module
  (import "fixpoint" "attach_tree_ro_table_0" (func $attach_tree_ro_table_0 (param externref)))
  (import "fixpoint" "attach_tree_ro_table_1" (func $attach_tree_ro_table_1 (param externref)))
  (import "fixpoint" "attach_blob_ro_mem_0" (func $attach_blob_ro_mem_0 (param externref)))
  (import "fixpoint" "size_ro_mem_0" (func $size_ro_mem_0 (result i32)))
  (import "fixpoint" "create_blob_i32" (func $create_blob_i32 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_0" (func $create_blob_rw_mem_0 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_1" (func $create_blob_rw_mem_1 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_2" (func $create_blob_rw_mem_2 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_3" (func $create_blob_rw_mem_3 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_4" (func $create_blob_rw_mem_4 (param i32) (result externref)))
  (import "fixpoint" "create_blob_rw_mem_5" (func $create_blob_rw_mem_5 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_0" (func $create_tree_rw_table_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_1" (func $create_tree_rw_table_1 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_2" (func $create_tree_rw_table_2 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_3" (func $create_tree_rw_table_3 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_4" (func $create_tree_rw_table_4 (param i32) (result externref)))
  (import "fixpoint" "create_thunk" (func $create_thunk (param externref) (result externref)))
  (import "fixpoint" "equality" (func $equality (param externref externref) (result i32)))
  (import "fixpoint" "create_tag" (func $create_tag (param externref externref) (result externref)))
  (import "fixpoint" "get_value_type" (func $get_value_type (param externref) (result i32)))
  (memory $ro_mem_0 (export "ro_mem_0") 0)
  (table $ro_table_0 (export "ro_table_0") 0 externref)
  (table $ro_table_1 (export "ro_table_1") 0 externref)
  (memory (export "rw_mem_0") (data "Runnable"))
  (memory (export "rw_mem_1") (data "Error"))
  (memory (export "rw_mem_2") (data "Invalid Bootstrap"))
  (memory (export "rw_mem_3") (data "Invalid Continuation"))
  (memory (export "rw_mem_4") (data "Okay"))
  (memory (export "rw_mem_5") (data "Continuation"))
  (table $rw_table_0 (export "rw_table_0") 3 externref)
  (table $rw_table_1 (export "rw_table_1") 4 externref)
  (table $rw_table_2 (export "rw_table_2") 4 externref)
  (table $rw_table_3 (export "rw_table_3") 3 externref)
  (table $rw_table_4 (export "rw_table_4") 3 externref)

  (global $parallelism (mut i32) (i32.const 0))

  ;; fn parse_encode(encode) -> (resource_limits, inner_encode, input)
  (func $parse_encode
    (param $encode externref)
    (result externref externref externref)
    (local $resource_limits externref)
    (local $input externref)
    (local $inner externref)
    (call $attach_tree_ro_table_0 (local.get $encode))
    ;; encode[0] is the resource limits
    (local.set $resource_limits (table.get $ro_table_0 (i32.const 0)))
    ;; encode[2] is input.wasm
    (local.set $input (table.get $ro_table_0 (i32.const 2)))
    (call $get_value_type (local.get $input))
    (i32.eq (i32.const 2))
    (if
      (then
        ;; if it's a Blob, get the size
        (call $attach_blob_ro_mem_0 (local.get $input))
        ;; input.size() >> 14
        (global.set $parallelism
          (i32.shr_u
            (call $size_ro_mem_0)
            (i32.const 14)))
      ))
    (local.set $inner (table.get $ro_table_0 (i32.const 1)))
    (local.get $resource_limits)
    (local.get $inner)
    (local.get $input))

  (func $do_compilation
    (param $resource_limits externref)
    (param $inner externref)
    (param $input externref)
    (result externref)
    (local $wasm2c externref)
    (local $cc externref)
    (local $ld externref)
    (local $system_deps externref)
    (local $clang_deps externref)
    (local $map externref)
    (local $compile externref)

    (call $attach_tree_ro_table_0 (local.get $inner))

    ;; inner[1] ought to be the fixed point, i.e., a tag authored by
    ;; compile.elf claiming that compile.elf itself is runnable
    (call $attach_tree_ro_table_1 (table.get $ro_table_0 (i32.const 1)))
    ;; inner[1][0] is compile.elf
    (local.set $compile (table.get $ro_table_1 (i32.const 0)))

    ;; inner[2] is the "bootstrap tag"; this is a special tag, authored
    ;; by compile.elf, marking a tree of resources as the string
    ;; "Bootstrap"
    ;; this tree contains the additional data we need
    (call $attach_tree_ro_table_0 (table.get $ro_table_0 (i32.const 2)))

    ;; check if the tag was authored by us
    (call $equality (local.get $compile) (table.get $ro_table_0 (i32.const 0)))

    (if (result externref)
      (then
        ;; the boot toolchain has a valid tag, extract the rest of the info and
        ;; run compilation

        ;; zoom into the boot toolchain tree
        ;; this contains: [wasm2c, cc, ld, system_deps, clang_deps, map]
        (call $attach_tree_ro_table_0 (table.get $ro_table_0 (i32.const 1)))
        (local.set $wasm2c (table.get $ro_table_0 (i32.const 0)))
        (local.set $cc (table.get $ro_table_0 (i32.const 1)))
        (local.set $ld (table.get $ro_table_0 (i32.const 2)))
        (local.set $system_deps (table.get $ro_table_0 (i32.const 3)))
        (local.set $clang_deps (table.get $ro_table_0 (i32.const 4)))
        (local.set $map (table.get $ro_table_0 (i32.const 5)))

        (call $make_compilation_thunk (local.get $resource_limits) (local.get $compile) (local.get $input) (local.get $wasm2c) (local.get $cc) (local.get $ld) (local.get $system_deps) (local.get $clang_deps) (local.get $map)))
      (else
        ;; invalid tag, return an error
        (call $error_invalid_boot_tree))))

  (func $make_compilation_thunk
    (param $resource_limits externref)
    (param $compile externref)
    (param $input externref)
    (param $wasm2c externref)
    (param $cc externref)
    (param $ld externref)
    (param $system_deps externref)
    (param $clang_deps externref)
    (param $map externref)
    (result externref)

    (local $c_files externref)
    (local $curried_cc externref)
    (local $o_files externref)
    (local $elf_file externref)
    (local $tag externref)
    (local $runnable externref)

    ;; c_files = wasm2c(input)
    (table.set $rw_table_0 (i32.const 0) (local.get $resource_limits))
    (table.set $rw_table_0 (i32.const 1) (local.get $wasm2c))
    (table.set $rw_table_0 (i32.const 2) (local.get $input))
    (call $create_tree_rw_table_0 (i32.const 3)) (call $create_thunk)

    (local.set $c_files)

    ;; curried_cc = lambda x: cc(system_deps, clang_deps, x)
    (table.set $rw_table_1 (i32.const 0) (local.get $resource_limits))
    (table.set $rw_table_1 (i32.const 1) (local.get $cc))
    (table.set $rw_table_1 (i32.const 2) (local.get $system_deps))
    (table.set $rw_table_1 (i32.const 3) (local.get $clang_deps))
    (call $create_tree_rw_table_1 (i32.const 4))
    (local.set $curried_cc)

    ;; o_files = map(curried_cc, c_files)
    (table.set $rw_table_2 (i32.const 0) (local.get $resource_limits))
    (table.set $rw_table_2 (i32.const 1) (local.get $map))
    (table.set $rw_table_2 (i32.const 2) (local.get $curried_cc))
    (table.set $rw_table_2 (i32.const 3) (local.get $c_files))
    (call $create_tree_rw_table_2 (i32.const 4))
    (call $create_thunk)
    (local.set $o_files)

    ;; elf_file = ld(o_files)
    (table.set $rw_table_3 (i32.const 0) (local.get $resource_limits))
    (table.set $rw_table_3 (i32.const 1) (local.get $ld))
    (table.set $rw_table_3 (i32.const 2) (local.get $o_files))
    (call $create_tree_rw_table_3 (i32.const 3))
    (call $create_thunk)
    (local.set $elf_file)

    ;; result = compile(tag(elf_file, "Continuation"))
    ;;
    ;; We cheat a bit here by creating a new tag that says the compile ELF is
    ;; runnable, instead of extracting it from the compile encode. Luckily,
    ;; since we're in Compile right now, Fixpoint will accept this as a valid
    ;; Runnable tag.
    (table.set $rw_table_4 (i32.const 0) (local.get $resource_limits))
    (table.set $rw_table_4 (i32.const 1) (call $tag_runnable (local.get $compile)))
    (table.set $rw_table_4 (i32.const 2) (call $tag_continuation (local.get $elf_file)))
    (call $create_tree_rw_table_4 (i32.const 3))
    (call $create_thunk))

  (func $error_invalid_boot_tree (result externref)
    (local $error externref)
    (local $msg externref)
    ;; create a tag of type "Error" with the message "Invalid Bootstrap"
    (local.set $error (call $create_blob_rw_mem_1 (i32.const 5)))
    (local.set $msg (call $create_blob_rw_mem_2 (i32.const 17)))
    (call $create_tag (local.get $msg) (local.get $error)))

  (func $error_invalid_continuation (result externref)
    (local $error externref)
    (local $msg externref)
    ;; create a tag of type "Error" with the message "Invalid Bootstrap"
    (local.set $error (call $create_blob_rw_mem_1 (i32.const 5)))
    (local.set $msg (call $create_blob_rw_mem_3 (i32.const 20)))
    (call $create_tag (local.get $msg) (local.get $error)))

  (func $error_not_runnable (param $msg externref) (result externref)
    (local $error externref)
    ;; create a tag of type "Error" with the error message
    (local.set $error (call $create_blob_rw_mem_1 (i32.const 5)))
    (call $create_tag (local.get $msg) (local.get $error)))

  (func $tag_runnable (param $ref externref) (result externref)
     (local.get $ref)
     (call $create_blob_rw_mem_0 (i32.const 8))
     (call $create_tag))

   (func $tag_continuation (param $ref externref) (result externref)
      (local.get $ref)
      (call $create_blob_rw_mem_5 (i32.const 12))
      (call $create_tag))

  ;; This function has different behavior depending on the type of the input.
  ;; If it's a Blob, it'll call itself on the result of compiling it. If it's a
  ;; Tag, it'll ensure the tag was created by itself; then it'll look at the
  ;; contents. The contents themselves should be a Tag created by some part of
  ;; the compilation toolchain, either marking an ELF file as "Okay" or an
  ;; error message as "[Tool] Error". It'll transform the "Okay" case into a
  ;; "Runnable" Tag, or recreate the Tag with itself as the author in the Error
  ;; case.
  (func (export "_fixpoint_apply")
    (param $encode externref) (result externref)
    (local $resource_limits externref)
    (local $inner_encode externref)
    (local $input externref)

    (call $parse_encode (local.get $encode))
    (local.set $input)
    (local.set $inner_encode)
    (local.set $resource_limits)

    (call $get_value_type (local.get $input))
    (i32.eq (i32.const 2))
    (if (result externref)
      (then
        ;; Blob
        (call $do_compilation (local.get $resource_limits) (local.get $inner_encode) (local.get $input)))
      (else
        ;; not a Blob
        (call $do_validate (local.get $inner_encode) (local.get $input))
      )))

  (func $do_validate (param $inner externref) (param $input externref) (result externref)
      (local $compile externref)
      (local $result externref)

      ;; get the name of this ELF file
      (call $attach_tree_ro_table_0 (local.get $inner))
      (local.set $compile (table.get $ro_table_0 (i32.const 1)))

      (call $get_value_type (local.get $input))
      (i32.eq (i32.const 3))
      (if
        (then
          ;; Tag
          )
        (else
          ;; Not a Tag
          (call $error_invalid_continuation)
          (return)))

      ;; Attach the Tag
      (call $attach_tree_ro_table_0 (local.get $input))
      ;; check if the tag was authored by us
      (call $equality (local.get $compile) (table.get $ro_table_0 (i32.const 0)))
      (if
        (then
          ;; Created by compile
          )
        (else
          ;; Not valid
          (call $error_invalid_continuation)
          (return)))
      ;; TODO: check that it's a tag of type "continuation"

      ;; The tag we created contains a tag created by some step in the process;
      ;; we need to ensure that's an "Okay" tag.
      (local.set $result (table.get $ro_table_0 (i32.const 1)))
      (call $attach_tree_ro_table_0 (local.get $result))
      (call $equality
        (call $create_blob_rw_mem_4 (i32.const 4))
        (table.get $ro_table_0 (i32.const 2)))
      (if
        (then
          ;; Okay
        )
        (else
          ;; Not valid, tag the object as Error
          (call $error_not_runnable (table.get $ro_table_0 (i32.const 1)))
          (return)))
      ;; Valid, tag the object as Runnable
      (call $tag_runnable (table.get $ro_table_0 (i32.const 1)))
    )
)
