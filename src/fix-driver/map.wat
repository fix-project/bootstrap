;; Arguments: (resource limits, map.elf, procedure.elf, Result<(x, args), error msg>)
;; Returns: Result<[forall i: thunk -> (resource limits, procedure, x, arg[i])], error msg>
(module
  (type (;0;) (func (param externref)))
  (import "fixpoint" "attach_tree_ro_table_0" (func $attach_tree_ro_table_0 (type 0)))
  (import "fixpoint" "attach_tree_ro_table_1" (func $attach_tree_ro_table_1 (type 0)))
  (import "fixpoint" "create_blob_rw_mem_0" (func $create_blob_rw_mem_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_0" (func $create_tree_rw_table_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_1" (func $create_tree_rw_table_1 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_2" (func $create_tree_rw_table_2 (param i32) (result externref)))
  (import "fixpoint" "create_application_thunk" (func $create_application_thunk (param externref) (result externref)))
  (import "fixpoint" "create_identification_thunk" (func $create_identification_thunk (param externref) (result externref)))
  (import "fixpoint" "create_strict_encode" (func $create_strict_encode (param externref) (result externref)))
  (import "fixpoint" "is_equal" (func $is_equal (param externref externref) (result i32)))
  (import "fixpoint" "create_tag" (func $create_tag (param externref externref) (result externref)))
  (import "fixpoint" "get_length" (func $get_length (param externref) (result i32)))
  (import "fixpoint" "create_blob_i32" (func $create_blob_i32 (param i32) (result externref)))
  (import "fixpoint" "create_blob_i64" (func $create_blob_i64 (param i64) (result externref)))
  (table $ro_table_0 (export "ro_table_0") 0 externref)
  (table $ro_table_1 (export "ro_table_1") 0 externref)
  (table $rw_table_0 (export "rw_table_0") 4 externref)
  (table $rw_table_1 (export "rw_table_1") 1 externref)
  (table $rw_table_2 (export "rw_table_2") 3 externref)
  (memory (export "rw_mem_0") (data "Okay"))

  ;; fn create_resource_limits(memory_usage, output_size, output_fan_out) -> externref
  (func $create_resource_limits
    (param $allowed_memory i64)
    (param $estimated_output_size i32)
    (param $estimated_fanout i32)
    (result externref)
    (local $allowed_memory_fix externref)
    (local.set $allowed_memory_fix (call $create_blob_i64 (local.get $allowed_memory)))
    (table.grow $rw_table_2 (local.get $allowed_memory_fix) (i32.const 3))
    drop
    (table.set $rw_table_2 (i32.const 0) (local.get $allowed_memory_fix))
    (table.set $rw_table_2 (i32.const 1) (call $create_blob_i32 (local.get $estimated_output_size)))
    (table.set $rw_table_2 (i32.const 2) (call $create_blob_i32 (local.get $estimated_fanout)))
    (call $create_tree_rw_table_2 (i32.const 3))
  )

  (func (export "_fixpoint_apply") (param $encode externref) (result externref) (local $i i32) (local $x externref)
    (local $okay externref)
    (call $attach_tree_ro_table_0 (local.get $encode))
    ;; {x, args}

    ;; attach the result tag, which could be an Okay or an Error
    (call $attach_tree_ro_table_1 (table.get $ro_table_0 (i32.const 3)))
    (local.set $okay (call $create_blob_rw_mem_0 (i32.const 4)))

    (call $is_equal
      (table.get $ro_table_1 (i32.const 2))
      (local.get $okay))
    (if
      (then
        ;; Okay
        )
      (else
        ;; Error
        (return (table.get $ro_table_0 (i32.const 3)))
      ))
    (call $attach_tree_ro_table_1 (table.get $ro_table_1 (i32.const 1)))

    (local.set $x (table.get $ro_table_1 (i32.const 0)))

    ;; attach args to ro_table_1
    (call $attach_tree_ro_table_1 (table.get $ro_table_1 (i32.const 1)))

    (table.grow $rw_table_1 (table.get $ro_table_0 (i32.const 0)) (i32.sub (table.size $ro_table_1) (i32.const 1)))
    drop

    (local.set $i (i32.const 0))
    (loop $ro_table_1_loop
          (table.grow $rw_table_0 (table.get $ro_table_0 (i32.const 0)) (i32.sub (i32.const 4) (table.size $rw_table_0)))
          drop
          ;; r
          (table.set $rw_table_0
            (i32.const 0)
            (call $create_resource_limits
                ;; memory usage 1024 * 1024 * 1024
                (i64.const 1073741824)
                ;; estimated output_size: c-file-size / 4 
                (i32.shr_u
                  (call $get_length (table.get $ro_table_1 (local.get $i)))
                  (i32.const 2))
                ;; estimated output_fan_out
                (i32.const 1)))
          ;; procedure
          (table.set $rw_table_0 (i32.const 1) (call $create_strict_encode (call $create_identification_thunk (table.get $ro_table_0 (i32.const 2)))))
          ;; x
          (table.set $rw_table_0 (i32.const 2) (local.get $x))
          ;; arg[i]
          (table.set $rw_table_0 (i32.const 3) (table.get $ro_table_1 (local.get $i)))
          (table.set $rw_table_1 (local.get $i) (call $create_application_thunk (call $create_tree_rw_table_0 (i32.const 4))))
          ;; i++
          (local.set $i (i32.add (local.get $i) (i32.const 1)))
          (br_if $ro_table_1_loop (i32.lt_s (local.get $i) (table.size $ro_table_1))))
    (call $create_tag
      (call $create_tree_rw_table_1 (table.size $ro_table_1))
      (local.get $okay))))
