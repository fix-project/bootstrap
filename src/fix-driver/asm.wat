(module
  (import "clang" "memory" (memory $program_mem 0))
  (memory $elfcontent (export "rw_mem_0") 1)
  (memory $stderr (export "rw_mem_1") 8)
  (memory $composer (export "rw_mem_2") 1)
  (memory $wasmcontent (export "ro_mem_0") 0)
  (table $input (export "ro_table_0") 0 externref)
  (table $clang_deps (export "ro_table_1") 0 externref)
  (table $return (export "rw_table_0") 4 externref)
  (table $rw_table_1 (export "rw_table_1") 257 externref)
  ;; access program memory
  (func (export "get_program_i32") (param $offset i32) (result i32)
    (local.get $offset)
    (i32.load $program_mem)
  )
  (func (export "set_program_i32") (param $offset i32) (param $val i32)
    (i32.store $program_mem (local.get $offset) (local.get $val))
  )

  ;; rw_0
  (func (export "program_memory_to_rw_0") (param $offset i32) (param $ptr i32) (param $len i32)
    (memory.copy $program_mem $elfcontent
    (local.get $offset)
    (local.get $ptr)
    (local.get $len))
  )
  (func (export "grow_rw_0") (param $size i32) (result i32)
    (memory.grow $elfcontent
    (local.get $size))
  )
  ;; rw_1
  (func (export "program_memory_to_rw_1") (param $offset i32) (param $ptr i32) (param $len i32)
    (memory.copy $program_mem $stderr
    (local.get $offset)
    (local.get $ptr)
    (local.get $len))
  )
  (func (export "grow_rw_1") (param $size i32) (result i32)
    (memory.grow $stderr
    (local.get $size))
  )
  ;; rw_2
  (func (export "program_memory_to_rw_2") (param $offset i32) (param $ptr i32) (param $len i32)
    (memory.copy $program_mem $composer
    (local.get $offset)
    (local.get $ptr)
    (local.get $len))
  )
  (func (export "grow_rw_2") (param $size i32) (result i32)
    (memory.grow $composer
    (local.get $size))
  )
  ;; ro_0
  (func (export "ro_0_to_program_memory") (param $offset i32) (param $ptr i32) (param $len i32)
    (memory.copy $wasmcontent $program_mem
    (local.get $offset)
    (local.get $ptr)
    (local.get $len))
  )
  ;; ro_table_0
  (func (export "get_ro_table_0") (param $index i32) (result externref)
    (table.get $input (local.get $index))
  )
  (func (export "size_ro_table_0") (result i32)
    (table.size $input)
  )
  ;; ro_table_1
  (func (export "get_ro_table_1") (param $index i32) (result externref)
    (table.get $clang_deps (local.get $index))
  )
  (func (export "size_ro_table_1") (result i32)
    (table.size $clang_deps)
  )
  ;; rw_table_0
  (func (export "set_rw_table_0") (param $index i32) (param $val externref)
    (table.set $return (local.get $index) (local.get $val))
  )
  (func (export "grow_rw_table_0") (param $size i32) (param $init_val externref ) (result i32)
    (local.get $init_val)
    (local.get $size)
    (table.grow $return)
  )
  ;; rw_table_1
  (func (export "set_rw_table_1") (param $index i32) (param $val externref)
    (table.set $rw_table_1 (local.get $index) (local.get $val))
  )
  (func (export "grow_rw_table_1") (param $size i32) (param $init_val externref ) (result i32)
    (local.get $init_val)
    (local.get $size)
    (table.grow $rw_table_1)
  )

)
