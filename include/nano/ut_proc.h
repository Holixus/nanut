#ifndef NANUT_PROC_H
#define NANUT_PROC_H

int proc_waitpid(pid_t pid);

pid_t proc_forkcmdf(char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));

int   proc_cmdf    (char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));
int   proc_qcmdf   (char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));
int   proc_lcmdf   (char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));

int   proc_sncmdf(char *str, size_t size, char const *fmt, ...) __attribute__ ((format (printf, 3, 4)));

int   proc_systemf (char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));

char const *proc_name();


int proc_killall(char const *proc, int sig);

pid_t proc_find_by_cmd(char const *cmd);

int proc_kill_pid_file(int signal, char const *fmt, ...) __attribute__ ((format (printf, 2, 3)));

pid_t proc_find_by_name(char const *name);

int trace_pid(int pid, int deep);

#endif
