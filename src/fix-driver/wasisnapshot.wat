(module
  (type (;0;) (func (param i32 i32 i32 i32) (result i32)))
  (type (;1;) (func (param i32) (result i32)))
  (type (;2;) (func (param i32 i32 i32)))
  (type (;3;) (func (param i32 i32)))
  (type (;4;) (func (param i32)))
  (type (;5;) (func (param i32) (result externref)))
  (type (;6;) (func (param i32 i32) (result i32)))
  (type (;7;) (func (param i32 i64 i32) (result i32)))
  (type (;8;) (func (param i32 i32 i32 i64 i32) (result i32)))
  (type (;9;) (func (param i32 i32 i32) (result i32)))
  (type (;10;) (func (param i32 i64 i32 i32) (result i32)))
  (type (;11;) (func (param i32 i32 i32 i32 i32) (result i32)))
  (type (;12;) (func (param i32 i32 i32 i32 i32 i64 i64 i32 i32) (result i32)))
  (type (;13;) (func (param i32 i32 i32 i32 i32 i32) (result i32)))
  (type (;14;) (func (param i32 i64) (result i32)))
  (import "asm" "get_program_i32" (func $get_program_i32 (type 1)))
  (import "asm" "program_memory_to_rw_1" (func $program_memory_to_rw_1 (type 2)))
  (import "asm" "set_program_i32" (func $set_program_i32 (type 3)))
  (import "fixpoint" "create_blob_rw_mem_1" (func $create_blob_rw_mem_1 (type 5)))
  (memory $localmemory 2)
  (func $fd_write (type 0) (param i32 i32 i32 i32) (result i32)
    (local i32 i32 i32)
    i32.const 8
    local.set 4
    block  ;; label = @1
      local.get 0
      i32.const 2
      i32.ne
      br_if 0 (;@1;)
      i32.const 0
      local.set 4
      i32.const 0
      local.set 5
      block  ;; label = @2
        local.get 2
        i32.const 1
        i32.lt_s
        br_if 0 (;@2;)
        i32.const 0
        local.set 5
        loop  ;; label = @3
          local.get 1
          call $get_program_i32
          local.set 6
          local.get 1
          i32.const 4
          i32.add
          call $get_program_i32
          local.set 0
          i32.const 0
          i32.load
          local.get 6
          local.get 0
          call $program_memory_to_rw_1
          i32.const 0
          local.get 0
          i32.const 0
          i32.load
          i32.add
          i32.store
          local.get 1
          i32.const 8
          i32.add
          local.set 1
          local.get 0
          local.get 5
          i32.add
          local.set 5
          local.get 2
          i32.const -1
          i32.add
          local.tee 2
          br_if 0 (;@3;)
        end
      end
      local.get 3
      local.get 5
      call $set_program_i32
    end
    local.get 4)
  (func $proc_exit (type 4) (param i32)
    i32.const 0
    i32.load
    call $create_blob_rw_mem_1
    drop)
  (func $environ_get (type 6) (param i32 i32) (result i32)
    i32.const 58)
  (func $environ_sizes_get (type 6) (param i32 i32) (result i32)
    local.get 0
    i32.const 0
    call $set_program_i32
    local.get 1
    i32.const 0
    call $set_program_i32
    i32.const 0)
  (func $clock_time_get (type 7) (param i32 i64 i32) (result i32)
    i32.const 58)
  (func $fd_close (type 1) (param i32) (result i32)
    (local i32)
    i32.const 8
    local.set 1
    block  ;; label = @1
      local.get 0
      i32.const 2
      i32.ne
      br_if 0 (;@1;)
      i32.const 0
      local.set 1
      i32.const 0
      i32.load
      call $create_blob_rw_mem_1
      drop
    end
    local.get 1)
  (func $fd_fdstat_get (type 6) (param i32 i32) (result i32)
    i32.const 8)
  (func $fd_fdstat_set_flags (type 6) (param i32 i32) (result i32)
    i32.const 8)
  (func $fd_filestat_get (type 6) (param i32 i32) (result i32)
    i32.const 8)
  (func $fd_filestat_set_size (type 14) (param i32 i64) (result i32)
    i32.const 8)
  (func $fd_pread (type 8) (param i32 i32 i32 i64 i32) (result i32)
    i32.const 8)
  (func $fd_prestat_get (type 6) (param i32 i32) (result i32)
    i32.const 8)
  (func $fd_prestat_dir_name (type 9) (param i32 i32 i32) (result i32)
    i32.const 8)
  (func $fd_read (type 0) (param i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $fd_readdir (type 8) (param i32 i32 i32 i64 i32) (result i32)
    i32.const 8)
  (func $fd_seek (type 10) (param i32 i64 i32 i32) (result i32)
    i32.const 8)
  (func $path_create_directory (type 9) (param i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_filestat_get (type 11) (param i32 i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_open (type 12) (param i32 i32 i32 i32 i32 i64 i64 i32 i32) (result i32)
    i32.const 8)
  (func $path_readlink (type 13) (param i32 i32 i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_remove_directory (type 9) (param i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_rename (type 13) (param i32 i32 i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_symlink (type 11) (param i32 i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $path_unlink_file (type 9) (param i32 i32 i32) (result i32)
    i32.const 8)
  (func $poll_oneoff (type 0) (param i32 i32 i32 i32) (result i32)
    i32.const 8)
  (func $random_get (type 6) (param i32 i32) (result i32)
    i32.const 58)
  (export "fd_write" (func $fd_write))
  (export "proc_exit" (func $proc_exit))
  (export "environ_get" (func $environ_get))
  (export "environ_sizes_get" (func $environ_sizes_get))
  (export "clock_time_get" (func $clock_time_get))
  (export "fd_close" (func $fd_close))
  (export "fd_fdstat_get" (func $fd_fdstat_get))
  (export "fd_fdstat_set_flags" (func $fd_fdstat_set_flags))
  (export "fd_filestat_get" (func $fd_filestat_get))
  (export "fd_filestat_set_size" (func $fd_filestat_set_size))
  (export "fd_pread" (func $fd_pread))
  (export "fd_prestat_get" (func $fd_prestat_get))
  (export "fd_prestat_dir_name" (func $fd_prestat_dir_name))
  (export "fd_read" (func $fd_read))
  (export "fd_readdir" (func $fd_readdir))
  (export "fd_seek" (func $fd_seek))
  (export "path_create_directory" (func $path_create_directory))
  (export "path_filestat_get" (func $path_filestat_get))
  (export "path_open" (func $path_open))
  (export "path_readlink" (func $path_readlink))
  (export "path_remove_directory" (func $path_remove_directory))
  (export "path_rename" (func $path_rename))
  (export "path_symlink" (func $path_symlink))
  (export "path_unlink_file" (func $path_unlink_file))
  (export "poll_oneoff" (func $poll_oneoff))
  (export "random_get" (func $random_get))
  (data $stderr_offset (i32.const 0) "\00\00\00\00"))
