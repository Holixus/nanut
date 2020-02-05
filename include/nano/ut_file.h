#ifndef NANO_UT_FILE_H
#define NANO_UT_FILE_H

int ut_read(int fd, char *data, int size);
int ut_write(int fd, char const *data, int size);
int ut_writef(int fd, char const *format, ...) __attribute__ ((format (printf, 2, 3)));

int ut_is_file_exist(char const *file_name);
int ut_is_file_empty(char const *file_name);

int ut_file_time(char const *file_name);

int ut_file_create(char const *file_name);
int ut_flag_file(char const *file_name, int yes);
int ut_file_flag_up(char const *file_name);

int ut_file_read(char const *file_name, char *buf, size_t size);
int ut_file_read_num(char const *file_name, int *num);
int ut_file_read_str(char const *file_name, char *buf, size_t size);
int ut_file_readf(char const *file_name, char const *format, ...) __attribute__ ((format (printf, 2, 3)));

int ut_file_write(char const *file_name, char const *buf, size_t size);
int ut_file_write_num(char const *file_name, long value);
int ut_file_write_str(char const *file_name, char const *str);
int ut_file_write_line(char const *file_name, char const *str);
int ut_file_writef(char const *file_name, char const *format, ...) __attribute__ ((format (printf, 2, 3)));

int ut_file_append(char const *file_name, char const *buf, size_t size);
int ut_file_append_str(char const *file_name, char const *str);
int ut_file_append_line(char const *file_name, char const *str);
int ut_file_appendf(char const *file_name, char const *format, ...) __attribute__ ((format (printf, 2, 3)));

ssize_t ut_file_copy(char const *dst_file, char const *src_file);
int ut_mkdirf(char const *format, ...) __attribute__ ((format (printf, 1, 2)));
int ut_mkpath(char const *path, int to_file);

#endif
