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

#include <signal.h>
#include <errno.h>
#include <syslog.h>

#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>

#include "nano/ut.h"
#include "nano/proc.h"
#include "nano/countof.h"

static int _execvp(char const *file, char *const argv[]) __attribute__ ((noreturn));
static void _vexecf(char const *fmt, va_list ap) __attribute__ ((noreturn));

/* ------------------------------------------------------------------------ */
static void _stdoutTo(int stdios, char const *file)
{
	if (file) {
		int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd < 0) {
			perror(file);
			exit(errno);
		}
		if (stdios & (1 << STDOUT_FILENO) || !stdios)
			dup2(fd, STDOUT_FILENO); /* copy the file descriptor fd into standard output */
		if (stdios & (1 << STDERR_FILENO) || !stdios)
			dup2(fd, STDERR_FILENO); /* same, for the standard error */
		close(fd);               /* close the file descriptor as we don't need it more  */
	} else {
#ifndef SYS_DEBUG
		close(2); // do not output error messages
#endif
	}
}


/* ------------------------------------------------------------------------ */
static int _execvp(char const *file, char *const argv[])
{
	signal(SIGINT, SIG_IGN);
	//setenv("PATH", "/bin", 1);
	execvp(file, argv);

	char args[256], *as = args, *end = args + sizeof args - 1;
	while (*argv && as < end)
		as += snprintf(as, (size_t)(end - as), "%s ", *argv++);

	*--as = 0;

	syslog(LOG_ERR, "execvp(%s, [%s]): %m", file, args);
	perror(file);
	exit(errno);
}


/* ------------------------------------------------------------------------ */
static int split_args(char *argv[], char const **redir, char *cmdline)
{
	int len = strlen(cmdline);

	if (cmdline[len - 1] == '&') {
		char *end = cmdline + len - 2;
		for (; end > cmdline && *end == ' '; --end)
			;
		end[1] = 0;
	}

	if (redir)
		*redir = NULL;

	char *_redir = strrchr(cmdline, '>');
	if (_redir && _redir != cmdline && !strchr(_redir, '\'') && !strchr(_redir, '"') && _redir[-1] != '\\') {
		char *b = _redir;
		do {
			*b = 0;
		} while (*--b == ' ');
		while (*++_redir == ' ')
			;
		//_stdoutTo(0, _redir);
		if (redir)
			*redir = _redir;
	}

	int argc = 0;
	char *p = cmdline, *d;
	while (*p) {
		while (*p == ' ')
			++p;
		argv[argc++] = p;
		for (d = p; *p; ) {
			char c = *p;
			if (c == ' ') {
				++p;
				break;
			}
			switch (c) {
			case '"':
			case '\'':;
				char q = c;
				++p;
				for (; *p; *d++ = *p++) {
					if (*p == '\\') {
						if (p[1])
							++p;
						continue;
					}
					if (*p == q) {
						++p;
						break;
					}
				}
				break;
			case '\\':
				if (p[1])
					++p;
			default:
				*d++ = *p++;
			}
		}
		*d = 0;
	}
	argv[argc] = NULL;
	return argc;
}


/* ------------------------------------------------------------------------ */
static void _vexecf(char const *fmt, va_list ap)
{
	char str[2048];
	char *argv[32];
	char const *fileout;

	vsnprintf(str, sizeof str, fmt, ap);

	split_args(argv, &fileout, str);

	if (fileout)
		_stdoutTo(0, fileout);

	_execvp(argv[0], argv);
}


/* ------------------------------------------------------------------------ */
int proc_waitpid(pid_t pid)
{
	if (pid < 0)
		return -1;

	int status, ret;
	do {
		ret = waitpid(pid, &status, 0);
	} while (ret < 0 && errno == EINTR);

	return ret < 0 ? ret : (WIFEXITED(status) ? WEXITSTATUS(status) : status);
}




/* ------------------------------------------------------------------------ */
pid_t proc_forkcmdf(char const *fmt, ...)
{
	if (!fmt || !fmt[0])
		return -1;

	pid_t pid = fork();
	if (pid)
		return pid;

	va_list ap;
	va_start(ap, fmt);
	_vexecf(fmt, ap);
}

/* ------------------------------------------------------------------------ */
int proc_cmdf(char const *fmt, ...)
{
	if (!fmt || !fmt[0])
		return -1;

	pid_t pid = fork();
	if (pid)
		return proc_waitpid(pid);

	va_list ap;
	va_start(ap, fmt);
	_vexecf(fmt, ap);
}

/* ------------------------------------------------------------------------ */
// redirect stderr & stdout to null // means quiet
int proc_qcmdf(char const *fmt, ...)
{
	if (!fmt || !fmt[0])
		return -1;

	pid_t pid = fork();
	if (pid)
		return proc_waitpid(pid);

	_stdoutTo(1 << STDERR_FILENO, "/dev/null");

	va_list ap;
	va_start(ap, fmt);
	_vexecf(fmt, ap);
}


/* ------------------------------------------------------------------------ */
static int sio_read(int std, char *buf, size_t size)
{
	int ret = 0;
	char *to = buf, *end = to + size - 1;
	int r;
	do {
		r = read(std, to, (size_t)(end - to));
		if (r >= 0) {
			to += r;
		} else {
			if (errno != EINTR) {
				ret = -1;
				break;
			}
		}
	} while (r && to < end);
	*to = 0;

	close(std);

	if (!ret)
		ret = to - buf;

	return ret;
}

/* ------------------------------------------------------------------------ */
// redirect stderr to syslog
static pid_t proc_lfork(char *argv[], char const *fileout)
{
	int serr[2];
	if (pipe(serr) < 0)
		return -1;

	pid_t pid = fork();
	if (pid < 0) {
		close(serr[1]);
		close(serr[0]);
		return -1;
	}

	if (!pid) {
		if (fileout)
			_stdoutTo(1 << STDOUT_FILENO, fileout);

		dup2(serr[1], STDERR_FILENO);
		close(serr[1]);
		close(serr[0]);

		execvp(argv[0], argv);
		syslog(LOG_ERR, "execvp(%s): %m", argv[0]);
		perror("execvp failed");
		_exit(1);
	}

	char errstr[512];
	close(serr[1]);
	sio_read(serr[0], errstr, sizeof errstr);

	int exitcode = proc_waitpid(pid);
	if (exitcode) {
		char cmd[512], *t = cmd, *e = cmd + sizeof cmd;
		cmd[1] = 0;
		for (; *argv; ++argv)
			t += snprintf(t, (size_t)(e - t), " %s", *argv);
		if (*errstr) {
			char *lf = strchr(errstr, '\n');
			if (lf)
				*lf = 0;
			syslog(LOG_ERR, "(%d)\"%s\" < %s ", exitcode, errstr, cmd + 1);
		} else {
			syslog(LOG_ERR, "(%d) %s", exitcode, cmd);
		}
	}
	return exitcode;
}


/* ------------------------------------------------------------------------ */
// redirect stderr to syslog
int proc_lcmdf(char const *fmt, ...)
{
	if (!fmt || !fmt[0])
		return -1;

	char cmd[2048];
	char *argv[32];
	char const *fileout;

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(cmd, sizeof cmd, fmt, ap);
	va_end(ap);

	split_args(argv, &fileout, cmd);

	return lfork(argv, fileout);
}



/* ------------------------------------------------------------------------ */
// redirect stdout to text buffer
static pid_t proc_snfork(char *str, size_t size, char *argv[])
{
	int sout[2];
	if (pipe(sout) < 0)
		return -1;

	pid_t pid = fork();
	if (pid < 0) {
		close(sout[1]);
		close(sout[0]);
		return -1;
	}

	if (!pid) {
		_stdoutTo(1 << STDERR_FILENO, "/dev/null");

		dup2(sout[1], STDOUT_FILENO);
		close(sout[1]);
		close(sout[0]);

		execvp(argv[0], argv);
		syslog(LOG_ERR, "execvp(%s): %m", argv[0]);
		perror("execvp failed");
		_exit(1);
	}

	close(sout[1]);
	sio_read(sout[0], str, size);

	return proc_waitpid(pid);
}


/* ------------------------------------------------------------------------ */
// redirect stdout to text buffer
int proc_sncmdf(char *str, size_t size, char const *fmt, ...)
{
	if (!fmt || !fmt[0])
		return -1;

	char cmd[2048];
	char *argv[32];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(cmd, sizeof cmd, fmt, ap);
	va_end(ap);

	split_args(argv, NULL, cmd);

	return snfork(str, size, argv);
}


/* ------------------------------------------------------------------------ */
// like system() but flexible
int proc_systemf(char const *format, ...)
{
	char str[2048];
	va_list ap;
	va_start(ap, format);
	vsnprintf(str, sizeof str, format, ap);
	va_end(ap);
	return system(str);
}


/* ------------------------------------------------------------------------ */
// returns name of current process from '/proc/%u/status; Name'
char const *proc_name()
{
	static char name[36];
	file_readf(strf("/proc/%u/status", getpid()), "Name: %32s", name);
	return name;
}


/* ------------------------------------------------------------------------ */
// send signal to all processes with '/proc/%u/status; Name' equal proc
int killall(char const *proc, int sig)
{
	static char const procfs[] = "/proc";
	DIR *dir = opendir(procfs);
	if (!dir)
		return -1;

	struct dirent *next;
	int counter = 0;
	int is_regex = !!strchr(proc, '|');
	char file[64];

	sprintf(file, "%s/", procfs);

	while ((next = readdir(dir)) != NULL) {
		char *d_name = next->d_name;

		/* If it isn't a number, we don't want it */
		if (d_name[0] < '0' || '9' < d_name[0])
			continue;

		sprintf(file + sizeof procfs, "%s/status", d_name);

		char status[64], name[64];
		if (readStrFromFile(file, status, sizeof status) < 0)
			continue ; // skip failed files

		sscanf(status, "Name: %s", name);

		if (is_regex) {
			char *p = (char *)proc;
			int len = strlen(name);
			for (; p && (p = strstr(p, name)); p = strchr(p, '|'))
				if ((!p[len] || p[len] == '|') && (p == proc || p[-1] == '|')) {
					//printf("-- kill %s/%s\n", name, d_name);
					if (!sig || !kill((pid_t)strtol(d_name, NULL, 10), sig))
						++counter;
					break;
				}
		} else
			if (!strcmp(name, proc)) {
				if (!sig || !kill((pid_t)strtol(d_name, NULL, 10), sig))
					++counter;
			}
	}
	closedir(dir);
	return counter;
}


/* ------------------------------------------------------------------------ */
// find the command line(cmd) execution pid
pid_t proc_find_by_cmd(char const *cmd)
{
	static char const procfs[] = "/proc";
	DIR *dir = opendir(procfs);
	if (!dir)
		return -1;

	struct dirent *next;
	char file[64];
	sprintf(file, "%s/", procfs);

	while ((next = readdir(dir))) {
		char *d_name = next->d_name;

		/* If it isn't a number, we don't want it */
		if (d_name[0] < '0' || '9' < d_name[0])
			continue;

		sprintf(file + sizeof procfs, "%s/cmdline", d_name);

		char cmdline[256];
		int len = readStrFromFile(file, cmdline, sizeof cmdline);

		if (len > 0) {
			char *end = cmdline + len - 1, *p = cmdline;
			for (; p < end; ++p)
				if (!*p)
					*p = ' ';

			if (!strcmp(cmdline, cmd)) {
				closedir(dir);
				return atoi(d_name);
			}
		}
	}
	closedir(dir);
	return 0;
}


/* ------------------------------------------------------------------------ */
// send signal to process by its pid file
int proc_kill_pid_file(int signal, char const *fmt, ...)
{
	char name[256];

	va_list ap;
	va_start(ap, file);
	vsnprintf(name, sizeof name, file, ap);
	va_end(ap);

	pid_t pid = readNumFromFile(name);
	if (pid < 0)
		return 0; // like killed already

	if (pid > 0) {
		if (kill(pid, signal) < 0) {
			if (errno != ESRCH) // no process means killed already
				return -1;
		} else {
			int c = 0;
			for (;!kill(pid, 0); ++c) {
				safe_msleep(100);
				if (c > 2) {
					kill(pid, SIGKILL);
					break;
				}
			}
		}
	}

	unlink(name);
	return 1; // killed
}


/* ------------------------------------------------------------------------ */
// find process by name from '/proc/%u/status; Name'
pid_t proc_find_by_name(char const *name)
{
	static char const procfs[] = "/proc";
	DIR *dir = opendir(procfs);
	if (!dir)
		return -1;

	char file[64];
	sprintf(file, "%s/", procfs);

	struct dirent *next;
	while ((next = readdir(dir))) {
		char *d_name = next->d_name;
		/* If it isn't a number, we don't want it */
		if (d_name[0] < '0' || '9' < d_name[0])
			continue;

		char text[128], cmd[256];

		sprintf(file + sizeof procfs, "%s/status", d_name);

		if (readStrFromFile(file, text, sizeof text) < 0) {
			closedir(dir);
			return -1;
		}
		if (sscanf(text, "Name: %s %*s", cmd) < 1) {
			closedir(dir);
			return -1;
		}

		if (!strcmp(cmd, name)) {
			closedir(dir);
			return (pid_t)atoi(d_name);
		}
	}
	closedir(dir);
	return 0;
}


/* ------------------------------------------------------------------------ */
typedef
struct proc {
	pid_t pid, ppid;
	char cmd[128];
} proc_t;

/* ------------------------------------------------------------------------ */
static int get_pdata(proc_t *data)
{
	char text[128], cmd[256];
	char state;
	if (readStrFromFile(strf("/proc/%u/stat", data->pid), text, sizeof text) < 0)
		return -1;
	if (sscanf(text, "%u %s %c %u %*s", &data->pid, cmd, &state, &data->ppid) < 4)
		return -1;

	int len = readStrFromFile(strf("/proc/%u/cmdline", data->pid), data->cmd, sizeof data->cmd);

	if (len) {
		char *end = data->cmd + len - 1, *p = data->cmd;
		for (; p < end; ++p)
			if (!*p)
				*p = ' ';
	} else {
		char *cl = strchr(cmd + 1, ')');
		if (cl)
			*cl = 0;
		strcpy(data->cmd, cmd + 1);
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
// printf breadcrumbs of parent pids
int trace_pid(int pid, int deep)
{
	if (!deep)
		return 0;
	proc_t data;
	data.pid = pid;
	if (get_pdata(&data) < 0)
		return -1;
	if (data.ppid != 1) {
		trace_pid(data.ppid, deep - 1);
		printf("> ");
	}
	printf("[%2$u]'%1$s'", data.cmd, data.pid);
	return 0;
}


