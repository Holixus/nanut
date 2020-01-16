#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <string.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>

#include <signal.h>
#include <errno.h>
#include <syslog.h>

#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>

#include "nano/ut.h"
#include "nano/ut_file.h"

/* ------------------------------------------------------------------------ */
int ut_read(int fd, char *data, int size)
{
	int ret;
	do {
		ret = read(fd, data, size);
	} while (ret < 0 && errno == EINTR);
	return ret;
}


/* ------------------------------------------------------------------------ */
int ut_write(int fd, char const *data, int size)
{
	if (!size)
		return 0;

	char const *end = data + size;
	int ret;
	do {
		do {
			ret = write(fd, data, (unsigned)(end - data));
		} while (ret < 0 && errno == EINTR);

		if (ret < 0)
			return ret;
	} while ((data += ret) < end);

	return size;
}


/* ------------------------------------------------------------------------ */
int ut_is_file_exist(char const *file_name)
{
	struct stat status;
	return stat(file_name, &status) >= 0;
}


/* ------------------------------------------------------------------------ */
int ut_is_file_empty(char const *file_name)
{
	struct stat status;
	if (stat(file_name, &status) < 0)
		return -1;
	return status.st_size ? 0 : 1;
}


/* ------------------------------------------------------------------------ */
int ut_file_time(char const *file_name)
{
	struct stat status;
	struct tm *ptm;

	if (stat(file_name, &status) < 0)
		return -1;

	ptm = gmtime(&status.st_mtime );

	if (!ptm)
		return -1;

	return (ptm->tm_mon << (6+6+5+5))|(ptm->tm_mday << (6+6+5))|(ptm->tm_hour << (6+6))|(ptm->tm_min << 6)|(ptm->tm_sec);
}


/* ------------------------------------------------------------------------ */
int ut_file_read(char const *filename, char *buf, size_t size)
{
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		return -1;
	int ret = ut_read(fd, buf, size);
	close(fd);
	return ret;
}

/* ------------------------------------------------------------------------ */
int ut_file_read_num(char const *filename, int *num)
{
	char buf[32];
	int ret = ut_file_read(filename, buf, sizeof buf);
	if (ret >= 0 && num)
		*num = atoi(buf);
	return ret;
}


/* ------------------------------------------------------------------------ */
int ut_writef(int fd, char const *format, ...)
{
	char text[2048];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(text, sizeof text, format, ap);
	va_end(ap);
	return ut_write(fd, text, (size_t)len);
}


/* ------------------------------------------------------------------------ */
int ut_file_create(char const *filename)
{
	int fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	if (fd < 0)
		return -1;
	close(fd);
	return 0;
}


/* ------------------------------------------------------------------------ */
int ut_flag_file(char const *filename, int yes)
{
	return (yes ? ut_file_create : unlink)(filename);
}


/* ------------------------------------------------------------------------ */
int ut_file_flag_up(char const *filename)
{
	if (ut_is_file_exist(filename))
		return 0;
	return ut_file_create(filename) < 0 ? -1 : 1;
}

/* ------------------------------------------------------------------------ */
int ut_file_write(char const *filename, char const *buf, size_t size)
{
	if (!buf)
		return unlink(filename);

	int fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	if (fd < 0)
		return -1;
	int ret = ut_write(fd, buf, size);
	close(fd);
	return ret;
}


/* ------------------------------------------------------------------------ */
int ut_file_append(char const *filename, char const *buf, size_t size)
{
	if (!buf)
		return -1;

	int fd = open(filename, O_APPEND|O_CREAT|O_WRONLY, 0666);
	if (fd < 0)
		return -1;
	int ret = ut_write(fd, buf, size);
	close(fd);
	return ret;
}


/* ------------------------------------------------------------------------ */
int ut_file_write_num(char const *filename, long value)
{
	return ut_file_writef(filename, "%li\n", value);
}


/* ------------------------------------------------------------------------ */
int ut_file_write_str(char const *filename, char const *str)
{
	return ut_file_write(filename, str, strlen(str));
}


/* ------------------------------------------------------------------------ */
int ut_file_append_str(char const *filename, char const *str)
{
	if (!str)
		return 0;

	return ut_file_append(filename, str, strlen(str));
}


/* ------------------------------------------------------------------------ */
int ut_file_read_str(char const *filename, char *buf, size_t size)
{
	int r = ut_file_read(filename, buf, size-1);
	if (r >= 0)
		buf[r] = 0;
	return r;
}


/* ------------------------------------------------------------------------ */
int ut_file_writef(char const *filename, char const *format, ...)
{
	char text[2048];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(text, sizeof text, format, ap);
	va_end(ap);
	return ut_file_write(filename, text, (size_t)len);
}


/* ------------------------------------------------------------------------ */
int ut_file_appendf(char const *filename, char const *format, ...)
{
	char text[2048];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(text, sizeof text, format, ap);
	va_end(ap);
	return ut_file_append(filename, text, (size_t)len);
}


/* ------------------------------------------------------------------------ */
int ut_file_readf(char const *filename, char const *format, ...)
{
	char text[2048];
	int ret = ut_file_read_str(filename, text, sizeof text);
	if (ret < 0)
		return ret;
	va_list ap;
	va_start(ap, format);
	ret = vsscanf(text, format, ap);
	va_end(ap);
	return ret;
}


/* ------------------------------------------------------------------------ */
int ut_file_write_line(char const *filename, char const *str)
{
	return ut_file_writef(filename, "%s\n", str);
}


/* ------------------------------------------------------------------------ */
int ut_file_append_line(char const *filename, char const *str)
{
	if (!str)
		return 0;

	return ut_file_appendf(filename, "%s\n", str);
}


/* ------------------------------------------------------------------------ */
ssize_t ut_file_copy(char const *dst_file, char const *src_file)
{
	int sd = open(src_file, O_RDONLY);
	if (sd < 0)
		return -1;

	int dd = open(dst_file, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	if (dd < 0)
		return -1;

	off_t offset = 0;
	size_t size = 0x7ffff000;
	ssize_t snt;
	do {
		snt = sendfile(dd, sd, &offset, (size_t)(size - offset));
	} while (snt < 0 && errno == EINTR);

	close(dd);
	close(sd);
	return snt >= 0 ? snt : -1;
}


/* ------------------------------------------------------------------------ */
int ut_mkdirf(char const *format, ...)
{
	char dir[2048];
	va_list ap;
	va_start(ap, format);
	vsnprintf(dir, sizeof dir, format, ap);
	va_end(ap);
	return mkdir(dir, 0755);
}

