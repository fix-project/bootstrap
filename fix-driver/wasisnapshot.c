#include <stdint.h>

typedef char __attribute__((address_space(10))) * externref;

extern externref create_blob_rw_mem_1(int32_t)
    __attribute((import_module("fixpoint"),
                 import_name("create_blob_rw_mem_1")));
extern void program_memory_to_rw_1(int32_t, int32_t, int32_t)
    __attribute((import_module("asm"), import_name("program_memory_to_rw_1")));
extern int32_t get_program_i32(int32_t)
    __attribute((import_module("asm"), import_name("get_program_i32")));
extern void set_program_i32(int32_t, int32_t)
    __attribute((import_module("asm"), import_name("set_program_i32")));

static int stderr_offset = 0;

int32_t fd_write(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0)
    __attribute__((__export_name__("fd_write")));
int32_t fd_write(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0) {
  int32_t iobuf_offset, iobuf_len;
  int32_t total_written = 0;

  if (fd != 2)
    return 8;

  for (int32_t i = 0; i < iovs_len; i++) {
    iobuf_offset = get_program_i32(iovs + i * 8);
    iobuf_len = get_program_i32(iovs + 4 + i * 8);

    program_memory_to_rw_1(stderr_offset, iobuf_offset, iobuf_len);
    stderr_offset += iobuf_len;
    total_written += iobuf_len;
  }

  set_program_i32(retptr0, total_written);
  return 0;
}

void proc_exit(int32_t rval) __attribute__((__export_name__("proc_exit")));
void proc_exit(int32_t rval) {
  create_blob_rw_mem_1(stderr_offset);
  return;
}

int32_t environ_get(int32_t environ, int32_t environ_buf)
    __attribute__((__export_name__("environ_get")));
int32_t environ_get(int32_t environ, int32_t environ_buf) { return 58; }

int32_t environ_sizes_get(int32_t retptr0, int32_t retptr1)
    __attribute__((__export_name__("environ_sizes_get")));
int32_t environ_sizes_get(int32_t retptr0, int32_t retptr1) {
  set_program_i32(retptr0, 0);
  set_program_i32(retptr1, 0);
  return 0;
}

int32_t clock_time_get(int32_t id, int64_t precision, int32_t retptr0)
    __attribute__((__export_name__("clock_time_get")));
int32_t clock_time_get(int32_t id, int64_t precision, int32_t retptr0) {
  return 58;
}

int32_t fd_close(int32_t fd) __attribute__((__export_name__("fd_close")));
int32_t fd_close(int32_t fd) {
  if (fd != 2)
    return 8;

  create_blob_rw_mem_1(stderr_offset);
  return 0;
}

int32_t fd_fdstat_get(int32_t fd, int32_t retptr0)
    __attribute__((__export_name__("fd_fdstat_get")));
int32_t fd_fdstat_get(int32_t fd, int32_t retptr0) { return 8; }

int32_t fd_fdstat_set_flags(int32_t fd, int32_t flags)
    __attribute__((__export_name__("fd_fdstat_set_flags")));
int32_t fd_fdstat_set_flags(int32_t fd, int32_t flags) { return 8; }

int32_t fd_filestat_get(int32_t fd, int32_t retptr0)
    __attribute__((__export_name__("fd_filestat_get")));
int32_t fd_filestat_get(int32_t fd, int32_t retptr0) { return 8; }

int32_t fd_pread(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
                 int32_t retptr0) __attribute__((__export_name__("fd_pread")));
int32_t fd_pread(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
                 int32_t retptr0) {
  return 8;
}

int32_t fd_prestat_get(int32_t fd, int32_t retptr0)
    __attribute__((__export_name__("fd_prestat_get")));
int32_t fd_prestat_get(int32_t fd, int32_t retptr0) { return 8; }

int32_t fd_prestat_dir_name(int32_t fd, int32_t path, int32_t path_len)
    __attribute__((__export_name__("fd_prestat_dir_name")));
int32_t fd_prestat_dir_name(int32_t fd, int32_t path, int32_t path_len) {
  return 8;
}

int32_t fd_read(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0)
    __attribute__((__export_name__("fd_read")));
int32_t fd_read(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t retptr0) {
  return 8;
}

int32_t fd_readdir(int32_t fd, int32_t buf, int32_t buf_len, int64_t cookie,
                   int32_t retptr0)
    __attribute__((__export_name__("fd_readdir")));
int32_t fd_readdir(int32_t fd, int32_t buf, int32_t buf_len, int64_t cookie,
                   int32_t retptr0) {
  return 8;
}

int32_t fd_seek(int32_t fd, int64_t offset, int32_t whence, int32_t retptr0)
    __attribute__((__export_name__("fd_seek")));
int32_t fd_seek(int32_t fd, int64_t offset, int32_t whence, int32_t retptr0) {
  return 8;
}

int32_t path_create_directory(int32_t fd, int32_t path, int32_t path_len)
    __attribute__((__export_name__("path_create_directory")));
int32_t path_create_directory(int32_t fd, int32_t path, int32_t path_len) {
  return 8;
}

int32_t path_filestat_get(int32_t fd, int32_t flags, int32_t path,
                          int32_t path_len, int32_t retptr0)
    __attribute__((__export_name__("path_filestat_get")));
int32_t path_filestat_get(int32_t fd, int32_t flags, int32_t path,
                          int32_t path_len, int32_t retptr0) {
  return 8;
}

int32_t path_open(int32_t fd, int32_t dirflags, int32_t path, int32_t path_len,
                  int32_t oflags, int64_t fs_rights_base,
                  int64_t fs_rights_inheriting, int32_t fdflags,
                  int32_t retptr0)
    __attribute__((__export_name__("path_open")));
int32_t path_open(int32_t fd, int32_t dirflags, int32_t path, int32_t path_len,
                  int32_t oflags, int64_t fs_rights_base,
                  int64_t fs_rights_inheriting, int32_t fdflags,
                  int32_t retptr0) {
  return 8;
}

int32_t path_readlink(int32_t fd, int32_t path, int32_t path_len, int32_t buf,
                      int32_t buf_len, int32_t retptr0)
    __attribute__((__export_name__("path_readlink")));
int32_t path_readlink(int32_t fd, int32_t path, int32_t path_len, int32_t buf,
                      int32_t buf_len, int32_t retptr0) {
  return 8;
}

int32_t path_remove_directory(int32_t fd, int32_t path, int32_t path_len)
    __attribute__((__export_name__("path_remove_directory")));
int32_t path_remove_directory(int32_t fd, int32_t path, int32_t path_len) {
  return 8;
}

int32_t path_rename(int32_t fd, int32_t old_path, int32_t old_path_len,
                    int32_t new_fd, int32_t new_path, int32_t new_path_len)
    __attribute__((__export_name__("path_rename")));
int32_t path_rename(int32_t fd, int32_t old_path, int32_t old_path_len,
                    int32_t new_fd, int32_t new_path, int32_t new_path_len) {
  return 8;
}

int32_t path_symlink(int32_t old_path, int32_t old_path_len, int32_t fd,
                     int32_t new_path, int32_t new_path_len)
    __attribute__((__export_name__("path_symlink")));
int32_t path_symlink(int32_t old_path, int32_t old_path_len, int32_t fd,
                     int32_t new_path, int32_t new_path_len) {
  return 8;
}

int32_t path_unlink_file(int32_t fd, int32_t path, int32_t path_len)
    __attribute__((__export_name__("path_unlink_file")));
int32_t path_unlink_file(int32_t fd, int32_t path, int32_t path_len) {
  return 8;
}

int32_t poll_oneoff(int32_t in, int32_t out, int32_t nsubscriptions,
                    int32_t retptr0)
    __attribute__((__export_name__("poll_oneoff")));
int32_t poll_oneoff(int32_t in, int32_t out, int32_t nsubscriptions,
                    int32_t retptr0) {
  return 8;
}

int32_t random_get(int32_t buf, int32_t buf_len)
    __attribute__((__export_name__("random_get")));
int32_t random_get(int32_t buf, int32_t buf_len) { return 58; }
