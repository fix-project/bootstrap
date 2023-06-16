;; (r, {r, compile, {{wasm2c.elf, clang.elf, lld.elf, system_dep, clang_dep, map.elf}, compile.wat, "bootstrap"} }, input.wasm)
;; ==> thunk->(r, lld.elf, thunk->{r, map.elf, {r, clang.elf, system_dep, clang_dep}, thunk{r, wasm2c.elf, input.wasm}})
(module
  (import "fixpoint" "attach_tree_ro_table_0" (func $attach_tree_ro_table_0 (param externref)))
  (import "fixpoint" "attach_tree_ro_table_1" (func $attach_tree_ro_table_1 (param externref)))
  (import "fixpoint" "attach_blob_ro_mem_0" (func $attach_blob_ro_mem_0 (param externref)))
  (import "fixpoint" "size_ro_mem_0" (func $size_ro_mem_0 (result i32)))
  (import "fixpoint" "create_blob_rw_mem_0" (func $create_blob_rw_mem_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_0" (func $create_tree_rw_table_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_1" (func $create_tree_rw_table_1 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_2" (func $create_tree_rw_table_2 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_3" (func $create_tree_rw_table_3 (param i32) (result externref)))
  (import "fixpoint" "create_thunk" (func $create_thunk (param externref) (result externref)))
  (import "fixpoint" "equality" (func $equality (param externref externref) (result i32)))
  (import "fixpoint" "create_tag" (func $create_tag (param externref externref) (result externref)))
  (memory $ro_mem_0 (export "ro_mem_0") 0)
  (table $ro_table_0 (export "ro_table_0") 0 externref)
  (table $ro_table_1 (export "ro_table_1") 0 externref)
  (memory (export "rw_mem_0") (data "Runnable"))
  (table $rw_table_0 (export "rw_table_0") 3 externref)
  (table $rw_table_1 (export "rw_table_1") 4 externref)
  (table $rw_table_2 (export "rw_table_2") 4 externref)
  (table $rw_table_3 (export "rw_table_3") 3 externref)
  (func (export "_fixpoint_apply") (param $encode externref) (result externref) (local $r externref) (local $input externref) (local $procedure externref) (local $input_parallelism i32) (local $runnable externref)
        (call $attach_tree_ro_table_0 (local.get $encode))
        (local.set $r (table.get $ro_table_0 (i32.const 0)))
        (local.set $input (table.get $ro_table_0 (i32.const 2)))

        (call $attach_blob_ro_mem_0 (local.get $input))
        (call $size_ro_mem_0)
        i32.const 14
        i32.shr_u
        local.set $input_parallelism

        (call $create_blob_rw_mem_0 (i32.const 8))
        local.set $runnable

        ;; get name for compile
        (call $attach_tree_ro_table_0 (table.get $ro_table_0 (i32.const 1)))

        ;; ro_table_1: {compile.wat, compile.wat, runnable}
        (call $attach_tree_ro_table_1 (table.get $ro_table_0 (i32.const 1)))
        (local.set $procedure (table.get $ro_table_1 (i32.const 0)))

        ;; ro_table_0: {compile-tool-tree, compile.wat, bootstrap}
        (call $attach_tree_ro_table_0 (table.get $ro_table_0 (i32.const 2)))
        (call $equality (local.get $procedure) (table.get $ro_table_0 (i32.const 1)))
        (if (result externref)
          (then
            ;; ro_table_0: {wasm2c.elf-runnable, clang.elf-runnable, lld.elf-runnable, system_dep, clang_dep, map.elf-runnable}
            (call $attach_tree_ro_table_0 (table.get $ro_table_0 (i32.const 0)))
            ;; thunk->{r, wasm2c.elf, input.wasm}
            (table.set $rw_table_0 (i32.const 0) (local.get $r))
            (table.set $rw_table_0 (i32.const 1) (table.get $ro_table_0 (i32.const 0)))
            (table.set $rw_table_0 (i32.const 2) (local.get $input))
            ;; thunk->{r, map.elf, {clang.elf, system_dep, clang_dep}, thunk->rw_table_0}
            (table.set $rw_table_2 (i32.const 3) (call $create_thunk (call $create_tree_rw_table_0 (i32.const 3))))
            (table.set $rw_table_2 (i32.const 0) (local.get $r))
            (table.set $rw_table_2 (i32.const 1) (table.get $ro_table_0 (i32.const 5)))
            ;; {r, clang.elf, system_dep, clang_dep}
            (table.set $rw_table_1 (i32.const 0) (local.get $r))
            (table.set $rw_table_1 (i32.const 1) (table.get $ro_table_0 (i32.const 1)))
            (table.set $rw_table_1 (i32.const 2) (table.get $ro_table_0 (i32.const 3)))
            (table.set $rw_table_1 (i32.const 3) (table.get $ro_table_0 (i32.const 4)))
            (table.set $rw_table_2 (i32.const 2) (call $create_tree_rw_table_1 (i32.const 4)))
            ;; thunk->{r, map.elf, rw_table_1, thunk->rw_table_0}
            local.get $input_parallelism
            (if (result externref)
              (then
                ;; thunk->{r, lld.elf, thunk->rw_table_2}
                (table.set $rw_table_3 (i32.const 2) (call $create_thunk (call $create_tree_rw_table_2 (i32.const 4))))
                (table.set $rw_table_3 (i32.const 0) (local.get $r))
                (table.set $rw_table_3 (i32.const 1) (table.get $ro_table_0 (i32.const 2)))
                (call $create_thunk (call $create_tree_rw_table_3 (i32.const 3)))
                )
              (else (call $create_thunk (call $create_tree_rw_table_2 (i32.const 4)))
                )
              )
            local.get $runnable
            call $create_tag
            )
          (else
            unreachable)
          )
        )
  )

