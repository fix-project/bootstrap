(module
  (type (;0;) (func (param externref)))
  (import "fixpoint" "attach_tree_ro_table_0" (func $attach_tree_ro_table_0 (type 0)))
  (import "fixpoint" "attach_tree_ro_table_1" (func $attach_tree_ro_table_1 (type 0)))
  (import "fixpoint" "create_tree_rw_table_0" (func $create_tree_rw_table_0 (param i32) (result externref)))
  (import "fixpoint" "create_tree_rw_table_1" (func $create_tree_rw_table_1 (param i32) (result externref)))
  (import "fixpoint" "create_thunk" (func $create_thunk (param externref) (result externref)))
  (table $ro_table_0 (export "ro_table_0") 0 externref)
  (table $ro_table_1 (export "ro_table_1") 0 externref)
  (table $rw_table_0 (export "rw_table_0") 3 externref)
  (table $rw_table_1 (export "rw_table_1") 1 externref)
  (func (export "_fixpoint_apply") (param $encode externref) (result externref) (local $i i32)
    (call $attach_tree_ro_table_0 (local.get $encode))
    (call $attach_tree_ro_table_1 (table.get $ro_table_0 (i32.const 3)))
    (table.grow $rw_table_1 (table.get $ro_table_0 (i32.const 0)) (i32.sub (table.size $ro_table_1) (i32.const 1)))
    drop
    (loop $ro_table_1_loop
      (table.grow $rw_table_0 (table.get $ro_table_0 (i32.const 0)) (i32.sub (i32.const 3) (table.size $rw_table_0)))
      drop
      
      (table.set $rw_table_0 (i32.const 0) (table.get $ro_table_0 (i32.const 0)))
      (table.set $rw_table_0 (i32.const 1) (table.get $ro_table_0 (i32.const 2)))
      (table.set $rw_table_0 (i32.const 2) (table.get $ro_table_1 (local.get $i)))
      (table.set $rw_table_1 (local.get $i) (call $create_thunk (call $create_tree_rw_table_0 (i32.const 3))))

      (local.set $i (i32.add (local.get $i) (i32.const 1)))
      (br_if $ro_table_1_loop (i32.lt_s (local.get $i) (table.size $ro_table_1))))
    (call $create_tree_rw_table_1 (table.size $ro_table_1)))
)
